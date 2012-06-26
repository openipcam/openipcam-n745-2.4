# Generated automatically from Makefile.in by configure.
#
# Minimum environment and virtual path setup
#
SHELL		= /bin/sh
srcdir		= .
top_srcdir	= ../..


#
# Paths
#
prefix		= /usr/local
exec_prefix	= /usr/local
bindir		= ${exec_prefix}/bin
sbindir		= ${exec_prefix}/sbin
libdir		= ${exec_prefix}/lib
datadir		= ${prefix}/share
includedir	= ${prefix}/include/ucd-snmp
mandir		= ${prefix}/man
man1dir		= $(mandir)/man1
man3dir		= $(mandir)/man3
man5dir		= $(mandir)/man5
man8dir		= $(mandir)/man8
snmplibdir	= $(datadir)/snmp
mibdir		= $(snmplibdir)/mibs
persistentdir	= /tmp


#
# Programs
#
INSTALL		= /usr/bin/install -c
SED		= sed
#RANLIB		= ranlib
LN_S		= ln -s

#
# Compiler arguments
#
# CFLAGS		= -g -O2 -Dlinux
EXTRACPPFLAGS	= -x c
# LDFLAGS		=  

#
# Shared library commands (or not)
#
SHLIB_CFLAGS		= 
SHLIB_EXTENSION		= a
SHLIB_VERSION		= 
SHLIB_LDCONFIG_CMD	= :
SHLIB_LD_CMD		= $(AR) cr
SHLIB_LD_LIBS		= 
CC_RUNTIME_ARG		= 


CPPFLAGS=-I. -I../.. -I$(srcdir) -I$(srcdir)/../.. \
	-I$(srcdir)/../../snmplib -I$(srcdir)/.. -I..  $(SHLIB_CFLAGS) $(INCS)
# CC = gcc

# Need a special .c -> .o definition here to make sure we place the
# object files in the sub directory.

.c.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

OBJS =  misc/ipfwacc.o mibII/system_mib.o mibII/sysORTable.o mibII/at.o mibII/interfaces.o mibII/snmp_mib.o mibII/tcp.o mibII/icmp.o mibII/ip.o mibII/udp.o mibII/vacm_vars.o ucd-snmp/memory.o ucd-snmp/proc.o ucd-snmp/versioninfo.o ucd-snmp/pass.o ucd-snmp/pass_persist.o ucd-snmp/disk.o ucd-snmp/loadave.o ucd-snmp/extensible.o ucd-snmp/errormib.o ucd-snmp/registry.o ucd-snmp/file.o snmpv3/snmpEngine.o snmpv3/snmpMPDStats.o snmpv3/usmStats.o snmpv3/usmUser.o util_funcs.o mibII/var_route.o mibII/route_write.o 
SRCS =  misc/ipfwacc.c mibII/system_mib.c mibII/sysORTable.c mibII/at.c mibII/interfaces.c mibII/snmp_mib.c mibII/tcp.c mibII/icmp.c mibII/ip.c mibII/udp.c mibII/vacm_vars.c ucd-snmp/memory.c ucd-snmp/proc.c ucd-snmp/versioninfo.c ucd-snmp/pass.c ucd-snmp/pass_persist.c ucd-snmp/disk.c ucd-snmp/loadave.c ucd-snmp/extensible.c ucd-snmp/errormib.c ucd-snmp/registry.c ucd-snmp/file.c snmpv3/snmpEngine.c snmpv3/snmpMPDStats.c snmpv3/usmStats.c snmpv3/usmUser.c util_funcs.c mibII/var_route.c mibII/route_write.c 
# OBJS =  mibII/system_mib.o mibII/sysORTable.o mibII/at.o mibII/interfaces.o mibII/snmp_mib.o mibII/tcp.o mibII/icmp.o mibII/ip.o mibII/udp.o mibII/vacm_vars.o ucd-snmp/memory.o ucd-snmp/vmstat.o ucd-snmp/proc.o ucd-snmp/versioninfo.o ucd-snmp/pass.o ucd-snmp/pass_persist.o ucd-snmp/disk.o ucd-snmp/loadave.o ucd-snmp/extensible.o ucd-snmp/errormib.o ucd-snmp/registry.o ucd-snmp/file.o snmpv3/snmpEngine.o snmpv3/snmpMPDStats.o snmpv3/usmStats.o snmpv3/usmUser.o util_funcs.o mibII/var_route.o mibII/route_write.o 
# SRCS =  mibII/system_mib.c mibII/sysORTable.c mibII/at.c mibII/interfaces.c mibII/snmp_mib.c mibII/tcp.c mibII/icmp.c mibII/ip.c mibII/udp.c mibII/vacm_vars.c ucd-snmp/memory.c ucd-snmp/vmstat.c ucd-snmp/proc.c ucd-snmp/versioninfo.c ucd-snmp/pass.c ucd-snmp/pass_persist.c ucd-snmp/disk.c ucd-snmp/loadave.c ucd-snmp/extensible.c ucd-snmp/errormib.c ucd-snmp/registry.c ucd-snmp/file.c snmpv3/snmpEngine.c snmpv3/snmpMPDStats.c snmpv3/usmStats.c snmpv3/usmUser.c util_funcs.c mibII/var_route.c mibII/route_write.c 

all: $(OBJS)

TAGS: $(SRCS)
	etags *.c *.h

clean:
	rm -f ${OBJS}

depend: 
	makedepend $(CPPFLAGS) $(SRCS)

nosysdepend:
	makedepend $(CPPFLAGS) $(SRCS)
	perl -n -i.bak $(top_srcdir)/makenosysdepend.pl Makefile

makefileindepend: nosysdepend
	perl $(top_srcdir)/makefileindepend.pl

# DO NOT DELETE THIS LINE -- make depend depends on it.


mibII/at.o: mibII/at.h mibII/interfaces.h mibincl.h /usr/include/alloca.h
mibII/at.o: /usr/include/asm/param.h /usr/include/asm/socket.h
mibII/at.o: /usr/include/asm/sockios.h /usr/include/bits/byteswap.h
mibII/at.o: /usr/include/bits/endian.h /usr/include/bits/in.h
mibII/at.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
mibII/at.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
mibII/at.o: /usr/include/bits/sockaddr.h /usr/include/bits/socket.h
mibII/at.o: /usr/include/bits/stdio_lim.h /usr/include/bits/time.h
mibII/at.o: /usr/include/bits/types.h /usr/include/bits/waitflags.h
mibII/at.o: /usr/include/bits/waitstatus.h /usr/include/bits/wordsize.h
mibII/at.o: /usr/include/endian.h /usr/include/features.h
mibII/at.o: /usr/include/_G_config.h /usr/include/gconv.h
mibII/at.o: /usr/include/gnu/stubs.h /usr/include/libio.h
mibII/at.o: /usr/include/linux/limits.h /usr/include/linux/param.h
mibII/at.o: /usr/include/malloc.h /usr/include/net/if.h
mibII/at.o: /usr/include/netinet/in.h /usr/include/net/route.h
mibII/at.o: /usr/include/stdint.h /usr/include/stdio.h /usr/include/stdlib.h
mibII/at.o: /usr/include/string.h /usr/include/sys/cdefs.h
mibII/at.o: /usr/include/syslog.h /usr/include/sys/param.h
mibII/at.o: /usr/include/sys/select.h /usr/include/sys/socket.h
mibII/at.o: /usr/include/sys/syslog.h /usr/include/sys/sysmacros.h
mibII/at.o: /usr/include/sys/time.h /usr/include/sys/types.h
mibII/at.o: /usr/include/time.h /usr/include/wchar.h /usr/include/xlocale.h
mibII/at.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
mibII/at.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
mibII/at.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
mibII/at.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
mibII/icmp.o: mibII/icmp.h mibII/sysORTable.h mibincl.h /usr/include/alloca.h
mibII/icmp.o: /usr/include/asm/param.h /usr/include/asm/socket.h
mibII/icmp.o: /usr/include/asm/sockios.h /usr/include/bits/byteswap.h
mibII/icmp.o: /usr/include/bits/endian.h /usr/include/bits/in.h
mibII/icmp.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
mibII/icmp.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
mibII/icmp.o: /usr/include/bits/sockaddr.h /usr/include/bits/socket.h
mibII/icmp.o: /usr/include/bits/stdio_lim.h /usr/include/bits/time.h
mibII/icmp.o: /usr/include/bits/types.h /usr/include/bits/waitflags.h
mibII/icmp.o: /usr/include/bits/waitstatus.h /usr/include/bits/wordsize.h
mibII/icmp.o: /usr/include/endian.h /usr/include/features.h
mibII/icmp.o: /usr/include/_G_config.h /usr/include/gconv.h
mibII/icmp.o: /usr/include/gnu/stubs.h /usr/include/libio.h
mibII/icmp.o: /usr/include/linux/limits.h /usr/include/linux/param.h
mibII/icmp.o: /usr/include/malloc.h /usr/include/net/if.h
mibII/icmp.o: /usr/include/netinet/in.h /usr/include/netinet/in_systm.h
mibII/icmp.o: /usr/include/netinet/ip.h /usr/include/netinet/ip_icmp.h
mibII/icmp.o: /usr/include/stdint.h /usr/include/stdio.h /usr/include/stdlib.h
mibII/icmp.o: /usr/include/string.h /usr/include/sys/cdefs.h
mibII/icmp.o: /usr/include/syslog.h /usr/include/sys/param.h
mibII/icmp.o: /usr/include/sys/select.h /usr/include/sys/socket.h
mibII/icmp.o: /usr/include/sys/syslog.h /usr/include/sys/sysmacros.h
mibII/icmp.o: /usr/include/sys/time.h /usr/include/sys/types.h
mibII/icmp.o: /usr/include/time.h /usr/include/wchar.h /usr/include/xlocale.h
mibII/icmp.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
mibII/icmp.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
mibII/icmp.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
mibII/icmp.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
mibII/interfaces.o: mibII/interfaces.h mibII/sysORTable.h mibincl.h
mibII/interfaces.o: /usr/include/alloca.h /usr/include/asm/ioctl.h
mibII/interfaces.o: /usr/include/asm/ioctls.h /usr/include/asm/param.h
mibII/interfaces.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
mibII/interfaces.o: /usr/include/bits/byteswap.h /usr/include/bits/confname.h
mibII/interfaces.o: /usr/include/bits/endian.h
mibII/interfaces.o: /usr/include/bits/environments.h /usr/include/bits/fcntl.h
mibII/interfaces.o: /usr/include/bits/in.h /usr/include/bits/ioctls.h
mibII/interfaces.o: /usr/include/bits/ioctl-types.h
mibII/interfaces.o: /usr/include/bits/posix_opt.h
mibII/interfaces.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
mibII/interfaces.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
mibII/interfaces.o: /usr/include/bits/sockaddr.h /usr/include/bits/socket.h
mibII/interfaces.o: /usr/include/bits/stat.h /usr/include/bits/stdio_lim.h
mibII/interfaces.o: /usr/include/bits/time.h /usr/include/bits/types.h
mibII/interfaces.o: /usr/include/bits/waitflags.h
mibII/interfaces.o: /usr/include/bits/waitstatus.h
mibII/interfaces.o: /usr/include/bits/wordsize.h /usr/include/endian.h
mibII/interfaces.o: /usr/include/fcntl.h /usr/include/features.h
mibII/interfaces.o: /usr/include/_G_config.h /usr/include/gconv.h
mibII/interfaces.o: /usr/include/gnu/stubs.h /usr/include/libio.h
mibII/interfaces.o: /usr/include/linux/limits.h /usr/include/linux/param.h
mibII/interfaces.o: /usr/include/malloc.h /usr/include/net/if.h
mibII/interfaces.o: /usr/include/netinet/in.h /usr/include/netinet/in_systm.h
mibII/interfaces.o: /usr/include/netinet/ip.h /usr/include/net/route.h
mibII/interfaces.o: /usr/include/stdint.h /usr/include/stdio.h
mibII/interfaces.o: /usr/include/stdlib.h /usr/include/string.h
mibII/interfaces.o: /usr/include/sys/cdefs.h /usr/include/sys/ioctl.h
mibII/interfaces.o: /usr/include/syslog.h /usr/include/sys/param.h
mibII/interfaces.o: /usr/include/sys/select.h /usr/include/sys/socket.h
mibII/interfaces.o: /usr/include/sys/stat.h /usr/include/sys/syslog.h
mibII/interfaces.o: /usr/include/sys/sysmacros.h /usr/include/sys/time.h
mibII/interfaces.o: /usr/include/sys/ttydefaults.h /usr/include/sys/types.h
mibII/interfaces.o: /usr/include/time.h /usr/include/unistd.h
mibII/interfaces.o: /usr/include/wchar.h /usr/include/xlocale.h
mibII/interfaces.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
mibII/interfaces.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
mibII/interfaces.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
mibII/interfaces.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
mibII/ip.o: mibII/at.h mibII/interfaces.h mibII/ip.h mibII/route_write.h
mibII/ip.o: mibII/sysORTable.h mibII/var_route.h mibincl.h
mibII/ip.o: /usr/include/alloca.h /usr/include/asm/param.h
mibII/ip.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
mibII/ip.o: /usr/include/bits/byteswap.h /usr/include/bits/endian.h
mibII/ip.o: /usr/include/bits/in.h /usr/include/bits/pthreadtypes.h
mibII/ip.o: /usr/include/bits/sched.h /usr/include/bits/select.h
mibII/ip.o: /usr/include/bits/sigset.h /usr/include/bits/sockaddr.h
mibII/ip.o: /usr/include/bits/socket.h /usr/include/bits/stdio_lim.h
mibII/ip.o: /usr/include/bits/time.h /usr/include/bits/types.h
mibII/ip.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
mibII/ip.o: /usr/include/bits/wordsize.h /usr/include/endian.h
mibII/ip.o: /usr/include/features.h /usr/include/_G_config.h
mibII/ip.o: /usr/include/gconv.h /usr/include/gnu/stubs.h /usr/include/libio.h
mibII/ip.o: /usr/include/linux/limits.h /usr/include/linux/param.h
mibII/ip.o: /usr/include/malloc.h /usr/include/net/if.h
mibII/ip.o: /usr/include/netinet/in.h /usr/include/netinet/in_systm.h
mibII/ip.o: /usr/include/netinet/ip.h /usr/include/net/route.h
mibII/ip.o: /usr/include/stdint.h /usr/include/stdio.h /usr/include/stdlib.h
mibII/ip.o: /usr/include/string.h /usr/include/sys/cdefs.h
mibII/ip.o: /usr/include/syslog.h /usr/include/sys/param.h
mibII/ip.o: /usr/include/sys/select.h /usr/include/sys/socket.h
mibII/ip.o: /usr/include/sys/syslog.h /usr/include/sys/sysmacros.h
mibII/ip.o: /usr/include/sys/time.h /usr/include/sys/types.h
mibII/ip.o: /usr/include/time.h /usr/include/wchar.h /usr/include/xlocale.h
mibII/ip.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
mibII/ip.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
mibII/ip.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
mibII/ip.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
mibII/route_write.o: mibII/at.h mibII/ip.h mibII/route_write.h
mibII/route_write.o: mibII/var_route.h /usr/include/alloca.h
mibII/route_write.o: /usr/include/arpa/inet.h /usr/include/asm/errno.h
mibII/route_write.o: /usr/include/asm/ioctl.h /usr/include/asm/ioctls.h
mibII/route_write.o: /usr/include/asm/param.h /usr/include/asm/socket.h
mibII/route_write.o: /usr/include/asm/sockios.h /usr/include/bits/byteswap.h
mibII/route_write.o: /usr/include/bits/confname.h /usr/include/bits/endian.h
mibII/route_write.o: /usr/include/bits/environments.h
mibII/route_write.o: /usr/include/bits/errno.h /usr/include/bits/fcntl.h
mibII/route_write.o: /usr/include/bits/in.h /usr/include/bits/ioctls.h
mibII/route_write.o: /usr/include/bits/ioctl-types.h /usr/include/bits/netdb.h
mibII/route_write.o: /usr/include/bits/posix_opt.h
mibII/route_write.o: /usr/include/bits/pthreadtypes.h
mibII/route_write.o: /usr/include/bits/sched.h /usr/include/bits/select.h
mibII/route_write.o: /usr/include/bits/sigset.h /usr/include/bits/sockaddr.h
mibII/route_write.o: /usr/include/bits/socket.h /usr/include/bits/stat.h
mibII/route_write.o: /usr/include/bits/stdio_lim.h /usr/include/bits/types.h
mibII/route_write.o: /usr/include/bits/waitflags.h
mibII/route_write.o: /usr/include/bits/waitstatus.h
mibII/route_write.o: /usr/include/bits/wordsize.h /usr/include/ctype.h
mibII/route_write.o: /usr/include/endian.h /usr/include/errno.h
mibII/route_write.o: /usr/include/fcntl.h /usr/include/features.h
mibII/route_write.o: /usr/include/_G_config.h /usr/include/gconv.h
mibII/route_write.o: /usr/include/gnu/stubs.h /usr/include/libio.h
mibII/route_write.o: /usr/include/linux/errno.h /usr/include/linux/limits.h
mibII/route_write.o: /usr/include/linux/param.h /usr/include/netdb.h
mibII/route_write.o: /usr/include/netinet/in.h /usr/include/net/route.h
mibII/route_write.o: /usr/include/rpc/netdb.h /usr/include/stdint.h
mibII/route_write.o: /usr/include/stdio.h /usr/include/stdlib.h
mibII/route_write.o: /usr/include/string.h /usr/include/sys/cdefs.h
mibII/route_write.o: /usr/include/sys/file.h /usr/include/sys/ioctl.h
mibII/route_write.o: /usr/include/syslog.h /usr/include/sys/param.h
mibII/route_write.o: /usr/include/sys/select.h /usr/include/sys/socket.h
mibII/route_write.o: /usr/include/sys/stat.h /usr/include/sys/syslog.h
mibII/route_write.o: /usr/include/sys/sysmacros.h
mibII/route_write.o: /usr/include/sys/ttydefaults.h /usr/include/sys/types.h
mibII/route_write.o: /usr/include/time.h /usr/include/unistd.h
mibII/route_write.o: /usr/include/wchar.h /usr/include/xlocale.h
mibII/route_write.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
mibII/route_write.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
mibII/route_write.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
mibII/route_write.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
mibII/snmp_mib.o: mibII/snmp_mib.h mibII/sysORTable.h mibincl.h struct.h
mibII/snmp_mib.o: /usr/include/alloca.h /usr/include/asm/socket.h
mibII/snmp_mib.o: /usr/include/asm/sockios.h /usr/include/bits/byteswap.h
mibII/snmp_mib.o: /usr/include/bits/endian.h /usr/include/bits/in.h
mibII/snmp_mib.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
mibII/snmp_mib.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
mibII/snmp_mib.o: /usr/include/bits/sockaddr.h /usr/include/bits/socket.h
mibII/snmp_mib.o: /usr/include/bits/stdio_lim.h /usr/include/bits/time.h
mibII/snmp_mib.o: /usr/include/bits/types.h /usr/include/bits/waitflags.h
mibII/snmp_mib.o: /usr/include/bits/waitstatus.h /usr/include/bits/wordsize.h
mibII/snmp_mib.o: /usr/include/endian.h /usr/include/features.h
mibII/snmp_mib.o: /usr/include/_G_config.h /usr/include/gconv.h
mibII/snmp_mib.o: /usr/include/gnu/stubs.h /usr/include/libio.h
mibII/snmp_mib.o: /usr/include/malloc.h /usr/include/netinet/in.h
mibII/snmp_mib.o: /usr/include/stdint.h /usr/include/stdio.h
mibII/snmp_mib.o: /usr/include/stdlib.h /usr/include/string.h
mibII/snmp_mib.o: /usr/include/sys/cdefs.h /usr/include/syslog.h
mibII/snmp_mib.o: /usr/include/sys/select.h /usr/include/sys/socket.h
mibII/snmp_mib.o: /usr/include/sys/syslog.h /usr/include/sys/sysmacros.h
mibII/snmp_mib.o: /usr/include/sys/time.h /usr/include/sys/types.h
mibII/snmp_mib.o: /usr/include/time.h /usr/include/wchar.h
mibII/snmp_mib.o: /usr/include/xlocale.h
mibII/snmp_mib.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
mibII/snmp_mib.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
mibII/snmp_mib.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
mibII/snmp_mib.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
mibII/snmp_mib.o: util_funcs.h
mibII/sysORTable.o: mibII/sysORTable.h mibincl.h /usr/include/alloca.h
mibII/sysORTable.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
mibII/sysORTable.o: /usr/include/bits/byteswap.h /usr/include/bits/endian.h
mibII/sysORTable.o: /usr/include/bits/in.h /usr/include/bits/pthreadtypes.h
mibII/sysORTable.o: /usr/include/bits/sched.h /usr/include/bits/select.h
mibII/sysORTable.o: /usr/include/bits/sigset.h /usr/include/bits/sockaddr.h
mibII/sysORTable.o: /usr/include/bits/socket.h /usr/include/bits/stdio_lim.h
mibII/sysORTable.o: /usr/include/bits/time.h /usr/include/bits/types.h
mibII/sysORTable.o: /usr/include/bits/waitflags.h
mibII/sysORTable.o: /usr/include/bits/waitstatus.h
mibII/sysORTable.o: /usr/include/bits/wordsize.h /usr/include/endian.h
mibII/sysORTable.o: /usr/include/features.h /usr/include/_G_config.h
mibII/sysORTable.o: /usr/include/gconv.h /usr/include/gnu/stubs.h
mibII/sysORTable.o: /usr/include/libio.h /usr/include/malloc.h
mibII/sysORTable.o: /usr/include/netinet/in.h /usr/include/stdint.h
mibII/sysORTable.o: /usr/include/stdio.h /usr/include/stdlib.h
mibII/sysORTable.o: /usr/include/string.h /usr/include/sys/cdefs.h
mibII/sysORTable.o: /usr/include/syslog.h /usr/include/sys/select.h
mibII/sysORTable.o: /usr/include/sys/socket.h /usr/include/sys/syslog.h
mibII/sysORTable.o: /usr/include/sys/sysmacros.h /usr/include/sys/time.h
mibII/sysORTable.o: /usr/include/sys/types.h /usr/include/time.h
mibII/sysORTable.o: /usr/include/wchar.h /usr/include/xlocale.h
mibII/sysORTable.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
mibII/sysORTable.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
mibII/sysORTable.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
mibII/sysORTable.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
mibII/system_mib.o: mibII/sysORTable.h mibII/system_mib.h mibincl.h
mibII/system_mib.o: /usr/include/alloca.h /usr/include/asm/socket.h
mibII/system_mib.o: /usr/include/asm/sockios.h /usr/include/bits/byteswap.h
mibII/system_mib.o: /usr/include/bits/confname.h /usr/include/bits/endian.h
mibII/system_mib.o: /usr/include/bits/environments.h /usr/include/bits/in.h
mibII/system_mib.o: /usr/include/bits/posix_opt.h
mibII/system_mib.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
mibII/system_mib.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
mibII/system_mib.o: /usr/include/bits/sockaddr.h /usr/include/bits/socket.h
mibII/system_mib.o: /usr/include/bits/stdio_lim.h /usr/include/bits/time.h
mibII/system_mib.o: /usr/include/bits/types.h /usr/include/bits/utsname.h
mibII/system_mib.o: /usr/include/bits/waitflags.h
mibII/system_mib.o: /usr/include/bits/waitstatus.h
mibII/system_mib.o: /usr/include/bits/wordsize.h /usr/include/ctype.h
mibII/system_mib.o: /usr/include/endian.h /usr/include/features.h
mibII/system_mib.o: /usr/include/_G_config.h /usr/include/gconv.h
mibII/system_mib.o: /usr/include/gnu/stubs.h /usr/include/libio.h
mibII/system_mib.o: /usr/include/malloc.h /usr/include/netinet/in.h
mibII/system_mib.o: /usr/include/stdint.h /usr/include/stdio.h
mibII/system_mib.o: /usr/include/stdlib.h /usr/include/string.h
mibII/system_mib.o: /usr/include/sys/cdefs.h /usr/include/syslog.h
mibII/system_mib.o: /usr/include/sys/select.h /usr/include/sys/socket.h
mibII/system_mib.o: /usr/include/sys/syslog.h /usr/include/sys/sysmacros.h
mibII/system_mib.o: /usr/include/sys/time.h /usr/include/sys/types.h
mibII/system_mib.o: /usr/include/sys/utsname.h /usr/include/time.h
mibII/system_mib.o: /usr/include/unistd.h /usr/include/wchar.h
mibII/system_mib.o: /usr/include/xlocale.h
mibII/system_mib.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
mibII/system_mib.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
mibII/system_mib.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
mibII/system_mib.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
mibII/tcp.o: mibII/sysORTable.h mibII/tcp.h mibincl.h /usr/include/alloca.h
mibII/tcp.o: /usr/include/arpa/inet.h /usr/include/asm/param.h
mibII/tcp.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
mibII/tcp.o: /usr/include/bits/byteswap.h /usr/include/bits/confname.h
mibII/tcp.o: /usr/include/bits/endian.h /usr/include/bits/environments.h
mibII/tcp.o: /usr/include/bits/in.h /usr/include/bits/posix_opt.h
mibII/tcp.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
mibII/tcp.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
mibII/tcp.o: /usr/include/bits/sockaddr.h /usr/include/bits/socket.h
mibII/tcp.o: /usr/include/bits/stdio_lim.h /usr/include/bits/time.h
mibII/tcp.o: /usr/include/bits/types.h /usr/include/bits/waitflags.h
mibII/tcp.o: /usr/include/bits/waitstatus.h /usr/include/bits/wordsize.h
mibII/tcp.o: /usr/include/endian.h /usr/include/features.h
mibII/tcp.o: /usr/include/_G_config.h /usr/include/gconv.h
mibII/tcp.o: /usr/include/gnu/stubs.h /usr/include/libio.h
mibII/tcp.o: /usr/include/linux/limits.h /usr/include/linux/param.h
mibII/tcp.o: /usr/include/malloc.h /usr/include/net/if.h
mibII/tcp.o: /usr/include/netinet/in.h /usr/include/netinet/in_systm.h
mibII/tcp.o: /usr/include/netinet/ip.h /usr/include/netinet/tcp.h
mibII/tcp.o: /usr/include/net/route.h /usr/include/stdint.h
mibII/tcp.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/string.h
mibII/tcp.o: /usr/include/sys/cdefs.h /usr/include/syslog.h
mibII/tcp.o: /usr/include/sys/param.h /usr/include/sys/select.h
mibII/tcp.o: /usr/include/sys/socket.h /usr/include/sys/socketvar.h
mibII/tcp.o: /usr/include/sys/syslog.h /usr/include/sys/sysmacros.h
mibII/tcp.o: /usr/include/sys/time.h /usr/include/sys/types.h
mibII/tcp.o: /usr/include/time.h /usr/include/unistd.h /usr/include/wchar.h
mibII/tcp.o: /usr/include/xlocale.h
mibII/tcp.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
mibII/tcp.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
mibII/tcp.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
mibII/tcp.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
mibII/udp.o: mibII/sysORTable.h mibII/tcp.h mibII/udp.h mibincl.h
mibII/udp.o: /usr/include/alloca.h /usr/include/asm/param.h
mibII/udp.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
mibII/udp.o: /usr/include/bits/byteswap.h /usr/include/bits/endian.h
mibII/udp.o: /usr/include/bits/in.h /usr/include/bits/pthreadtypes.h
mibII/udp.o: /usr/include/bits/sched.h /usr/include/bits/select.h
mibII/udp.o: /usr/include/bits/sigset.h /usr/include/bits/sockaddr.h
mibII/udp.o: /usr/include/bits/socket.h /usr/include/bits/stdio_lim.h
mibII/udp.o: /usr/include/bits/time.h /usr/include/bits/types.h
mibII/udp.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
mibII/udp.o: /usr/include/bits/wordsize.h /usr/include/endian.h
mibII/udp.o: /usr/include/features.h /usr/include/_G_config.h
mibII/udp.o: /usr/include/gconv.h /usr/include/gnu/stubs.h
mibII/udp.o: /usr/include/libio.h /usr/include/linux/limits.h
mibII/udp.o: /usr/include/linux/param.h /usr/include/malloc.h
mibII/udp.o: /usr/include/net/if.h /usr/include/netinet/in.h
mibII/udp.o: /usr/include/netinet/in_systm.h /usr/include/netinet/ip.h
mibII/udp.o: /usr/include/netinet/udp.h /usr/include/net/route.h
mibII/udp.o: /usr/include/stdint.h /usr/include/stdio.h /usr/include/stdlib.h
mibII/udp.o: /usr/include/string.h /usr/include/sys/cdefs.h
mibII/udp.o: /usr/include/syslog.h /usr/include/sys/param.h
mibII/udp.o: /usr/include/sys/select.h /usr/include/sys/socket.h
mibII/udp.o: /usr/include/sys/socketvar.h /usr/include/sys/syslog.h
mibII/udp.o: /usr/include/sys/sysmacros.h /usr/include/sys/time.h
mibII/udp.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/wchar.h
mibII/udp.o: /usr/include/xlocale.h
mibII/udp.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
mibII/udp.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
mibII/udp.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
mibII/udp.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
mibII/vacm_vars.o: mibII/sysORTable.h mibII/vacm_vars.h mibincl.h
mibII/vacm_vars.o: /usr/include/alloca.h /usr/include/arpa/inet.h
mibII/vacm_vars.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
mibII/vacm_vars.o: /usr/include/bits/byteswap.h /usr/include/bits/confname.h
mibII/vacm_vars.o: /usr/include/bits/endian.h /usr/include/bits/environments.h
mibII/vacm_vars.o: /usr/include/bits/in.h /usr/include/bits/netdb.h
mibII/vacm_vars.o: /usr/include/bits/posix_opt.h
mibII/vacm_vars.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
mibII/vacm_vars.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
mibII/vacm_vars.o: /usr/include/bits/sockaddr.h /usr/include/bits/socket.h
mibII/vacm_vars.o: /usr/include/bits/stdio_lim.h /usr/include/bits/time.h
mibII/vacm_vars.o: /usr/include/bits/types.h /usr/include/bits/waitflags.h
mibII/vacm_vars.o: /usr/include/bits/waitstatus.h /usr/include/bits/wordsize.h
mibII/vacm_vars.o: /usr/include/ctype.h /usr/include/endian.h
mibII/vacm_vars.o: /usr/include/features.h /usr/include/_G_config.h
mibII/vacm_vars.o: /usr/include/gconv.h /usr/include/gnu/stubs.h
mibII/vacm_vars.o: /usr/include/libio.h /usr/include/malloc.h
mibII/vacm_vars.o: /usr/include/netdb.h /usr/include/netinet/in.h
mibII/vacm_vars.o: /usr/include/rpc/netdb.h /usr/include/stdint.h
mibII/vacm_vars.o: /usr/include/stdio.h /usr/include/stdlib.h
mibII/vacm_vars.o: /usr/include/string.h /usr/include/sys/cdefs.h
mibII/vacm_vars.o: /usr/include/syslog.h /usr/include/sys/select.h
mibII/vacm_vars.o: /usr/include/sys/socket.h /usr/include/sys/syslog.h
mibII/vacm_vars.o: /usr/include/sys/sysmacros.h /usr/include/sys/time.h
mibII/vacm_vars.o: /usr/include/sys/types.h /usr/include/time.h
mibII/vacm_vars.o: /usr/include/unistd.h /usr/include/wchar.h
mibII/vacm_vars.o: /usr/include/xlocale.h
mibII/vacm_vars.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
mibII/vacm_vars.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
mibII/vacm_vars.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
mibII/vacm_vars.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
mibII/var_route.o: mibII/at.h mibII/interfaces.h mibII/ip.h
mibII/var_route.o: mibII/route_write.h mibII/var_route.h struct.h
mibII/var_route.o: /usr/include/alloca.h /usr/include/arpa/inet.h
mibII/var_route.o: /usr/include/asm/param.h /usr/include/asm/socket.h
mibII/var_route.o: /usr/include/asm/sockios.h /usr/include/bits/byteswap.h
mibII/var_route.o: /usr/include/bits/endian.h /usr/include/bits/in.h
mibII/var_route.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
mibII/var_route.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
mibII/var_route.o: /usr/include/bits/sockaddr.h /usr/include/bits/socket.h
mibII/var_route.o: /usr/include/bits/stdio_lim.h /usr/include/bits/time.h
mibII/var_route.o: /usr/include/bits/types.h /usr/include/bits/waitflags.h
mibII/var_route.o: /usr/include/bits/waitstatus.h /usr/include/bits/wordsize.h
mibII/var_route.o: /usr/include/endian.h /usr/include/features.h
mibII/var_route.o: /usr/include/_G_config.h /usr/include/gconv.h
mibII/var_route.o: /usr/include/gnu/stubs.h /usr/include/libio.h
mibII/var_route.o: /usr/include/linux/limits.h /usr/include/linux/param.h
mibII/var_route.o: /usr/include/net/if.h /usr/include/netinet/in.h
mibII/var_route.o: /usr/include/net/route.h /usr/include/stdint.h
mibII/var_route.o: /usr/include/stdio.h /usr/include/stdlib.h
mibII/var_route.o: /usr/include/string.h /usr/include/sys/cdefs.h
mibII/var_route.o: /usr/include/syslog.h /usr/include/sys/param.h
mibII/var_route.o: /usr/include/sys/select.h /usr/include/sys/socket.h
mibII/var_route.o: /usr/include/sys/syslog.h /usr/include/sys/sysmacros.h
mibII/var_route.o: /usr/include/sys/time.h /usr/include/sys/types.h
mibII/var_route.o: /usr/include/time.h /usr/include/wchar.h
mibII/var_route.o: /usr/include/xlocale.h
mibII/var_route.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
mibII/var_route.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
mibII/var_route.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
mibII/var_route.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
mibII/var_route.o: util_funcs.h
misc/ipfwacc.o: mibincl.h misc/ipfwacc.h struct.h /usr/include/alloca.h
misc/ipfwacc.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
misc/ipfwacc.o: /usr/include/bits/byteswap.h /usr/include/bits/endian.h
misc/ipfwacc.o: /usr/include/bits/in.h /usr/include/bits/pthreadtypes.h
misc/ipfwacc.o: /usr/include/bits/sched.h /usr/include/bits/select.h
misc/ipfwacc.o: /usr/include/bits/sigset.h /usr/include/bits/sockaddr.h
misc/ipfwacc.o: /usr/include/bits/socket.h /usr/include/bits/stdio_lim.h
misc/ipfwacc.o: /usr/include/bits/time.h /usr/include/bits/types.h
misc/ipfwacc.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
misc/ipfwacc.o: /usr/include/bits/wordsize.h /usr/include/endian.h
misc/ipfwacc.o: /usr/include/features.h /usr/include/_G_config.h
misc/ipfwacc.o: /usr/include/gconv.h /usr/include/gnu/stubs.h
misc/ipfwacc.o: /usr/include/libio.h /usr/include/malloc.h
misc/ipfwacc.o: /usr/include/netinet/in.h /usr/include/stdint.h
misc/ipfwacc.o: /usr/include/stdio.h /usr/include/stdlib.h
misc/ipfwacc.o: /usr/include/sys/cdefs.h /usr/include/syslog.h
misc/ipfwacc.o: /usr/include/sys/select.h /usr/include/sys/socket.h
misc/ipfwacc.o: /usr/include/sys/syslog.h /usr/include/sys/sysmacros.h
misc/ipfwacc.o: /usr/include/sys/time.h /usr/include/sys/types.h
misc/ipfwacc.o: /usr/include/time.h /usr/include/wchar.h
misc/ipfwacc.o: /usr/include/xlocale.h
misc/ipfwacc.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
misc/ipfwacc.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
misc/ipfwacc.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
misc/ipfwacc.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
misc/ipfwacc.o: util_funcs.h
snmpv3/snmpEngine.o: mibincl.h snmpv3/snmpEngine.h struct.h
snmpv3/snmpEngine.o: /usr/include/alloca.h /usr/include/asm/socket.h
snmpv3/snmpEngine.o: /usr/include/asm/sockios.h /usr/include/bits/byteswap.h
snmpv3/snmpEngine.o: /usr/include/bits/endian.h /usr/include/bits/in.h
snmpv3/snmpEngine.o: /usr/include/bits/pthreadtypes.h
snmpv3/snmpEngine.o: /usr/include/bits/sched.h /usr/include/bits/select.h
snmpv3/snmpEngine.o: /usr/include/bits/sigset.h /usr/include/bits/sockaddr.h
snmpv3/snmpEngine.o: /usr/include/bits/socket.h /usr/include/bits/stdio_lim.h
snmpv3/snmpEngine.o: /usr/include/bits/time.h /usr/include/bits/types.h
snmpv3/snmpEngine.o: /usr/include/bits/waitflags.h
snmpv3/snmpEngine.o: /usr/include/bits/waitstatus.h
snmpv3/snmpEngine.o: /usr/include/bits/wordsize.h /usr/include/endian.h
snmpv3/snmpEngine.o: /usr/include/features.h /usr/include/_G_config.h
snmpv3/snmpEngine.o: /usr/include/gconv.h /usr/include/gnu/stubs.h
snmpv3/snmpEngine.o: /usr/include/libio.h /usr/include/malloc.h
snmpv3/snmpEngine.o: /usr/include/netinet/in.h /usr/include/stdint.h
snmpv3/snmpEngine.o: /usr/include/stdio.h /usr/include/stdlib.h
snmpv3/snmpEngine.o: /usr/include/sys/cdefs.h /usr/include/syslog.h
snmpv3/snmpEngine.o: /usr/include/sys/select.h /usr/include/sys/socket.h
snmpv3/snmpEngine.o: /usr/include/sys/syslog.h /usr/include/sys/sysmacros.h
snmpv3/snmpEngine.o: /usr/include/sys/time.h /usr/include/sys/types.h
snmpv3/snmpEngine.o: /usr/include/time.h /usr/include/wchar.h
snmpv3/snmpEngine.o: /usr/include/xlocale.h
snmpv3/snmpEngine.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
snmpv3/snmpEngine.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
snmpv3/snmpEngine.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
snmpv3/snmpEngine.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
snmpv3/snmpEngine.o: util_funcs.h
snmpv3/snmpMPDStats.o: mibincl.h snmpv3/snmpMPDStats.h struct.h
snmpv3/snmpMPDStats.o: /usr/include/alloca.h /usr/include/asm/socket.h
snmpv3/snmpMPDStats.o: /usr/include/asm/sockios.h /usr/include/bits/byteswap.h
snmpv3/snmpMPDStats.o: /usr/include/bits/endian.h /usr/include/bits/in.h
snmpv3/snmpMPDStats.o: /usr/include/bits/pthreadtypes.h
snmpv3/snmpMPDStats.o: /usr/include/bits/sched.h /usr/include/bits/select.h
snmpv3/snmpMPDStats.o: /usr/include/bits/sigset.h /usr/include/bits/sockaddr.h
snmpv3/snmpMPDStats.o: /usr/include/bits/socket.h
snmpv3/snmpMPDStats.o: /usr/include/bits/stdio_lim.h /usr/include/bits/time.h
snmpv3/snmpMPDStats.o: /usr/include/bits/types.h /usr/include/bits/waitflags.h
snmpv3/snmpMPDStats.o: /usr/include/bits/waitstatus.h
snmpv3/snmpMPDStats.o: /usr/include/bits/wordsize.h /usr/include/endian.h
snmpv3/snmpMPDStats.o: /usr/include/features.h /usr/include/_G_config.h
snmpv3/snmpMPDStats.o: /usr/include/gconv.h /usr/include/gnu/stubs.h
snmpv3/snmpMPDStats.o: /usr/include/libio.h /usr/include/malloc.h
snmpv3/snmpMPDStats.o: /usr/include/netinet/in.h /usr/include/stdint.h
snmpv3/snmpMPDStats.o: /usr/include/stdio.h /usr/include/stdlib.h
snmpv3/snmpMPDStats.o: /usr/include/sys/cdefs.h /usr/include/syslog.h
snmpv3/snmpMPDStats.o: /usr/include/sys/select.h /usr/include/sys/socket.h
snmpv3/snmpMPDStats.o: /usr/include/sys/syslog.h /usr/include/sys/sysmacros.h
snmpv3/snmpMPDStats.o: /usr/include/sys/time.h /usr/include/sys/types.h
snmpv3/snmpMPDStats.o: /usr/include/time.h /usr/include/wchar.h
snmpv3/snmpMPDStats.o: /usr/include/xlocale.h
snmpv3/snmpMPDStats.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
snmpv3/snmpMPDStats.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
snmpv3/snmpMPDStats.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
snmpv3/snmpMPDStats.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
snmpv3/snmpMPDStats.o: util_funcs.h
snmpv3/usmStats.o: mibincl.h snmpv3/usmStats.h struct.h /usr/include/alloca.h
snmpv3/usmStats.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
snmpv3/usmStats.o: /usr/include/bits/byteswap.h /usr/include/bits/endian.h
snmpv3/usmStats.o: /usr/include/bits/in.h /usr/include/bits/pthreadtypes.h
snmpv3/usmStats.o: /usr/include/bits/sched.h /usr/include/bits/select.h
snmpv3/usmStats.o: /usr/include/bits/sigset.h /usr/include/bits/sockaddr.h
snmpv3/usmStats.o: /usr/include/bits/socket.h /usr/include/bits/stdio_lim.h
snmpv3/usmStats.o: /usr/include/bits/time.h /usr/include/bits/types.h
snmpv3/usmStats.o: /usr/include/bits/waitflags.h
snmpv3/usmStats.o: /usr/include/bits/waitstatus.h /usr/include/bits/wordsize.h
snmpv3/usmStats.o: /usr/include/endian.h /usr/include/features.h
snmpv3/usmStats.o: /usr/include/_G_config.h /usr/include/gconv.h
snmpv3/usmStats.o: /usr/include/gnu/stubs.h /usr/include/libio.h
snmpv3/usmStats.o: /usr/include/malloc.h /usr/include/netinet/in.h
snmpv3/usmStats.o: /usr/include/stdint.h /usr/include/stdio.h
snmpv3/usmStats.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
snmpv3/usmStats.o: /usr/include/syslog.h /usr/include/sys/select.h
snmpv3/usmStats.o: /usr/include/sys/socket.h /usr/include/sys/syslog.h
snmpv3/usmStats.o: /usr/include/sys/sysmacros.h /usr/include/sys/time.h
snmpv3/usmStats.o: /usr/include/sys/types.h /usr/include/time.h
snmpv3/usmStats.o: /usr/include/wchar.h /usr/include/xlocale.h
snmpv3/usmStats.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
snmpv3/usmStats.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
snmpv3/usmStats.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
snmpv3/usmStats.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
snmpv3/usmStats.o: util_funcs.h
snmpv3/usmUser.o: mibincl.h snmpv3/usmUser.h struct.h /usr/include/alloca.h
snmpv3/usmUser.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
snmpv3/usmUser.o: /usr/include/bits/byteswap.h /usr/include/bits/endian.h
snmpv3/usmUser.o: /usr/include/bits/in.h /usr/include/bits/pthreadtypes.h
snmpv3/usmUser.o: /usr/include/bits/sched.h /usr/include/bits/select.h
snmpv3/usmUser.o: /usr/include/bits/sigset.h /usr/include/bits/sockaddr.h
snmpv3/usmUser.o: /usr/include/bits/socket.h /usr/include/bits/stdio_lim.h
snmpv3/usmUser.o: /usr/include/bits/time.h /usr/include/bits/types.h
snmpv3/usmUser.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
snmpv3/usmUser.o: /usr/include/bits/wordsize.h /usr/include/endian.h
snmpv3/usmUser.o: /usr/include/features.h /usr/include/_G_config.h
snmpv3/usmUser.o: /usr/include/gconv.h /usr/include/gnu/stubs.h
snmpv3/usmUser.o: /usr/include/libio.h /usr/include/malloc.h
snmpv3/usmUser.o: /usr/include/netinet/in.h /usr/include/stdint.h
snmpv3/usmUser.o: /usr/include/stdio.h /usr/include/stdlib.h
snmpv3/usmUser.o: /usr/include/string.h /usr/include/sys/cdefs.h
snmpv3/usmUser.o: /usr/include/syslog.h /usr/include/sys/select.h
snmpv3/usmUser.o: /usr/include/sys/socket.h /usr/include/sys/syslog.h
snmpv3/usmUser.o: /usr/include/sys/sysmacros.h /usr/include/sys/time.h
snmpv3/usmUser.o: /usr/include/sys/types.h /usr/include/time.h
snmpv3/usmUser.o: /usr/include/wchar.h /usr/include/xlocale.h
snmpv3/usmUser.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
snmpv3/usmUser.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
snmpv3/usmUser.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
snmpv3/usmUser.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
snmpv3/usmUser.o: util_funcs.h
ucd-snmp/disk.o: mibdefs.h mibincl.h struct.h ucd-snmp/disk.h
ucd-snmp/disk.o: ucd-snmp/errormib.h /usr/include/alloca.h
ucd-snmp/disk.o: /usr/include/asm/errno.h /usr/include/asm/page.h
ucd-snmp/disk.o: /usr/include/asm/param.h /usr/include/asm/sigcontext.h
ucd-snmp/disk.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
ucd-snmp/disk.o: /usr/include/bits/byteswap.h /usr/include/bits/confname.h
ucd-snmp/disk.o: /usr/include/bits/endian.h /usr/include/bits/environments.h
ucd-snmp/disk.o: /usr/include/bits/errno.h /usr/include/bits/fcntl.h
ucd-snmp/disk.o: /usr/include/bits/in.h /usr/include/bits/posix_opt.h
ucd-snmp/disk.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
ucd-snmp/disk.o: /usr/include/bits/select.h /usr/include/bits/sigaction.h
ucd-snmp/disk.o: /usr/include/bits/sigcontext.h /usr/include/bits/siginfo.h
ucd-snmp/disk.o: /usr/include/bits/signum.h /usr/include/bits/sigset.h
ucd-snmp/disk.o: /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h
ucd-snmp/disk.o: /usr/include/bits/sockaddr.h /usr/include/bits/socket.h
ucd-snmp/disk.o: /usr/include/bits/stat.h /usr/include/bits/stdio_lim.h
ucd-snmp/disk.o: /usr/include/bits/time.h /usr/include/bits/types.h
ucd-snmp/disk.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
ucd-snmp/disk.o: /usr/include/bits/wordsize.h /usr/include/endian.h
ucd-snmp/disk.o: /usr/include/errno.h /usr/include/fcntl.h
ucd-snmp/disk.o: /usr/include/features.h /usr/include/_G_config.h
ucd-snmp/disk.o: /usr/include/gconv.h /usr/include/gnu/stubs.h
ucd-snmp/disk.o: /usr/include/libio.h /usr/include/linux/errno.h
ucd-snmp/disk.o: /usr/include/linux/limits.h /usr/include/linux/param.h
ucd-snmp/disk.o: /usr/include/malloc.h /usr/include/netinet/in.h
ucd-snmp/disk.o: /usr/include/signal.h /usr/include/stdint.h
ucd-snmp/disk.o: /usr/include/stdio.h /usr/include/stdlib.h
ucd-snmp/disk.o: /usr/include/string.h /usr/include/sys/cdefs.h
ucd-snmp/disk.o: /usr/include/syslog.h /usr/include/sys/param.h
ucd-snmp/disk.o: /usr/include/sys/select.h /usr/include/sys/socket.h
ucd-snmp/disk.o: /usr/include/sys/stat.h /usr/include/sys/syslog.h
ucd-snmp/disk.o: /usr/include/sys/sysmacros.h /usr/include/sys/time.h
ucd-snmp/disk.o: /usr/include/sys/types.h /usr/include/sys/ucontext.h
ucd-snmp/disk.o: /usr/include/time.h /usr/include/ucontext.h
ucd-snmp/disk.o: /usr/include/unistd.h /usr/include/wchar.h
ucd-snmp/disk.o: /usr/include/xlocale.h
ucd-snmp/disk.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
ucd-snmp/disk.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
ucd-snmp/disk.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
ucd-snmp/disk.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
ucd-snmp/disk.o: util_funcs.h
ucd-snmp/errormib.o: mibdefs.h mibincl.h struct.h ucd-snmp/errormib.h
ucd-snmp/errormib.o: /usr/include/alloca.h /usr/include/asm/errno.h
ucd-snmp/errormib.o: /usr/include/asm/page.h /usr/include/asm/param.h
ucd-snmp/errormib.o: /usr/include/asm/sigcontext.h /usr/include/asm/socket.h
ucd-snmp/errormib.o: /usr/include/asm/sockios.h /usr/include/bits/byteswap.h
ucd-snmp/errormib.o: /usr/include/bits/confname.h /usr/include/bits/endian.h
ucd-snmp/errormib.o: /usr/include/bits/environments.h
ucd-snmp/errormib.o: /usr/include/bits/errno.h /usr/include/bits/fcntl.h
ucd-snmp/errormib.o: /usr/include/bits/in.h /usr/include/bits/posix_opt.h
ucd-snmp/errormib.o: /usr/include/bits/pthreadtypes.h
ucd-snmp/errormib.o: /usr/include/bits/sched.h /usr/include/bits/select.h
ucd-snmp/errormib.o: /usr/include/bits/sigaction.h
ucd-snmp/errormib.o: /usr/include/bits/sigcontext.h
ucd-snmp/errormib.o: /usr/include/bits/siginfo.h /usr/include/bits/signum.h
ucd-snmp/errormib.o: /usr/include/bits/sigset.h /usr/include/bits/sigstack.h
ucd-snmp/errormib.o: /usr/include/bits/sigthread.h
ucd-snmp/errormib.o: /usr/include/bits/sockaddr.h /usr/include/bits/socket.h
ucd-snmp/errormib.o: /usr/include/bits/stat.h /usr/include/bits/stdio_lim.h
ucd-snmp/errormib.o: /usr/include/bits/time.h /usr/include/bits/types.h
ucd-snmp/errormib.o: /usr/include/bits/waitflags.h
ucd-snmp/errormib.o: /usr/include/bits/waitstatus.h
ucd-snmp/errormib.o: /usr/include/bits/wordsize.h /usr/include/endian.h
ucd-snmp/errormib.o: /usr/include/errno.h /usr/include/fcntl.h
ucd-snmp/errormib.o: /usr/include/features.h /usr/include/_G_config.h
ucd-snmp/errormib.o: /usr/include/gconv.h /usr/include/gnu/stubs.h
ucd-snmp/errormib.o: /usr/include/libio.h /usr/include/linux/errno.h
ucd-snmp/errormib.o: /usr/include/linux/limits.h /usr/include/linux/param.h
ucd-snmp/errormib.o: /usr/include/malloc.h /usr/include/netinet/in.h
ucd-snmp/errormib.o: /usr/include/signal.h /usr/include/stdint.h
ucd-snmp/errormib.o: /usr/include/stdio.h /usr/include/stdlib.h
ucd-snmp/errormib.o: /usr/include/string.h /usr/include/sys/cdefs.h
ucd-snmp/errormib.o: /usr/include/syslog.h /usr/include/sys/param.h
ucd-snmp/errormib.o: /usr/include/sys/select.h /usr/include/sys/socket.h
ucd-snmp/errormib.o: /usr/include/sys/stat.h /usr/include/sys/syslog.h
ucd-snmp/errormib.o: /usr/include/sys/sysmacros.h /usr/include/sys/time.h
ucd-snmp/errormib.o: /usr/include/sys/types.h /usr/include/sys/ucontext.h
ucd-snmp/errormib.o: /usr/include/time.h /usr/include/ucontext.h
ucd-snmp/errormib.o: /usr/include/unistd.h /usr/include/wchar.h
ucd-snmp/errormib.o: /usr/include/xlocale.h
ucd-snmp/errormib.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
ucd-snmp/errormib.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
ucd-snmp/errormib.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
ucd-snmp/errormib.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
ucd-snmp/errormib.o: util_funcs.h
ucd-snmp/extensible.o: mibdefs.h mibincl.h struct.h ucd-snmp/extensible.h
ucd-snmp/extensible.o: /usr/include/alloca.h /usr/include/asm/errno.h
ucd-snmp/extensible.o: /usr/include/asm/page.h /usr/include/asm/param.h
ucd-snmp/extensible.o: /usr/include/asm/sigcontext.h /usr/include/asm/socket.h
ucd-snmp/extensible.o: /usr/include/asm/sockios.h /usr/include/bits/byteswap.h
ucd-snmp/extensible.o: /usr/include/bits/confname.h /usr/include/bits/endian.h
ucd-snmp/extensible.o: /usr/include/bits/environments.h
ucd-snmp/extensible.o: /usr/include/bits/errno.h /usr/include/bits/fcntl.h
ucd-snmp/extensible.o: /usr/include/bits/in.h /usr/include/bits/posix_opt.h
ucd-snmp/extensible.o: /usr/include/bits/pthreadtypes.h
ucd-snmp/extensible.o: /usr/include/bits/sched.h /usr/include/bits/select.h
ucd-snmp/extensible.o: /usr/include/bits/sigaction.h
ucd-snmp/extensible.o: /usr/include/bits/sigcontext.h
ucd-snmp/extensible.o: /usr/include/bits/siginfo.h /usr/include/bits/signum.h
ucd-snmp/extensible.o: /usr/include/bits/sigset.h /usr/include/bits/sigstack.h
ucd-snmp/extensible.o: /usr/include/bits/sigthread.h
ucd-snmp/extensible.o: /usr/include/bits/sockaddr.h /usr/include/bits/socket.h
ucd-snmp/extensible.o: /usr/include/bits/stat.h /usr/include/bits/stdio_lim.h
ucd-snmp/extensible.o: /usr/include/bits/time.h /usr/include/bits/types.h
ucd-snmp/extensible.o: /usr/include/bits/waitflags.h
ucd-snmp/extensible.o: /usr/include/bits/waitstatus.h
ucd-snmp/extensible.o: /usr/include/bits/wordsize.h /usr/include/ctype.h
ucd-snmp/extensible.o: /usr/include/endian.h /usr/include/errno.h
ucd-snmp/extensible.o: /usr/include/fcntl.h /usr/include/features.h
ucd-snmp/extensible.o: /usr/include/_G_config.h /usr/include/gconv.h
ucd-snmp/extensible.o: /usr/include/gnu/stubs.h /usr/include/libio.h
ucd-snmp/extensible.o: /usr/include/linux/errno.h /usr/include/linux/limits.h
ucd-snmp/extensible.o: /usr/include/linux/param.h /usr/include/malloc.h
ucd-snmp/extensible.o: /usr/include/netinet/in.h /usr/include/signal.h
ucd-snmp/extensible.o: /usr/include/stdint.h /usr/include/stdio.h
ucd-snmp/extensible.o: /usr/include/stdlib.h /usr/include/string.h
ucd-snmp/extensible.o: /usr/include/sys/cdefs.h /usr/include/syslog.h
ucd-snmp/extensible.o: /usr/include/sys/param.h /usr/include/sys/select.h
ucd-snmp/extensible.o: /usr/include/sys/socket.h /usr/include/sys/stat.h
ucd-snmp/extensible.o: /usr/include/sys/syslog.h /usr/include/sys/sysmacros.h
ucd-snmp/extensible.o: /usr/include/sys/time.h /usr/include/sys/types.h
ucd-snmp/extensible.o: /usr/include/sys/ucontext.h /usr/include/time.h
ucd-snmp/extensible.o: /usr/include/ucontext.h /usr/include/unistd.h
ucd-snmp/extensible.o: /usr/include/wchar.h /usr/include/xlocale.h
ucd-snmp/extensible.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
ucd-snmp/extensible.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
ucd-snmp/extensible.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
ucd-snmp/extensible.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
ucd-snmp/extensible.o: util_funcs.h
ucd-snmp/file.o: mibdefs.h mibincl.h struct.h ucd-snmp/file.h
ucd-snmp/file.o: /usr/include/alloca.h /usr/include/asm/socket.h
ucd-snmp/file.o: /usr/include/asm/sockios.h /usr/include/bits/byteswap.h
ucd-snmp/file.o: /usr/include/bits/endian.h /usr/include/bits/in.h
ucd-snmp/file.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
ucd-snmp/file.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
ucd-snmp/file.o: /usr/include/bits/sockaddr.h /usr/include/bits/socket.h
ucd-snmp/file.o: /usr/include/bits/stat.h /usr/include/bits/stdio_lim.h
ucd-snmp/file.o: /usr/include/bits/time.h /usr/include/bits/types.h
ucd-snmp/file.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
ucd-snmp/file.o: /usr/include/bits/wordsize.h /usr/include/endian.h
ucd-snmp/file.o: /usr/include/features.h /usr/include/_G_config.h
ucd-snmp/file.o: /usr/include/gconv.h /usr/include/gnu/stubs.h
ucd-snmp/file.o: /usr/include/libio.h /usr/include/malloc.h
ucd-snmp/file.o: /usr/include/netinet/in.h /usr/include/stdint.h
ucd-snmp/file.o: /usr/include/stdio.h /usr/include/stdlib.h
ucd-snmp/file.o: /usr/include/sys/cdefs.h /usr/include/syslog.h
ucd-snmp/file.o: /usr/include/sys/select.h /usr/include/sys/socket.h
ucd-snmp/file.o: /usr/include/sys/stat.h /usr/include/sys/syslog.h
ucd-snmp/file.o: /usr/include/sys/sysmacros.h /usr/include/sys/time.h
ucd-snmp/file.o: /usr/include/sys/types.h /usr/include/time.h
ucd-snmp/file.o: /usr/include/wchar.h /usr/include/xlocale.h
ucd-snmp/file.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
ucd-snmp/file.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
ucd-snmp/file.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
ucd-snmp/file.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
ucd-snmp/file.o: util_funcs.h
ucd-snmp/loadave.o: mibdefs.h mibincl.h struct.h ucd-snmp/loadave.h
ucd-snmp/loadave.o: /usr/include/alloca.h /usr/include/asm/errno.h
ucd-snmp/loadave.o: /usr/include/asm/page.h /usr/include/asm/param.h
ucd-snmp/loadave.o: /usr/include/asm/sigcontext.h /usr/include/asm/socket.h
ucd-snmp/loadave.o: /usr/include/asm/sockios.h /usr/include/bits/byteswap.h
ucd-snmp/loadave.o: /usr/include/bits/confname.h /usr/include/bits/endian.h
ucd-snmp/loadave.o: /usr/include/bits/environments.h /usr/include/bits/errno.h
ucd-snmp/loadave.o: /usr/include/bits/fcntl.h /usr/include/bits/in.h
ucd-snmp/loadave.o: /usr/include/bits/posix_opt.h
ucd-snmp/loadave.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
ucd-snmp/loadave.o: /usr/include/bits/select.h /usr/include/bits/sigaction.h
ucd-snmp/loadave.o: /usr/include/bits/sigcontext.h /usr/include/bits/siginfo.h
ucd-snmp/loadave.o: /usr/include/bits/signum.h /usr/include/bits/sigset.h
ucd-snmp/loadave.o: /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h
ucd-snmp/loadave.o: /usr/include/bits/sockaddr.h /usr/include/bits/socket.h
ucd-snmp/loadave.o: /usr/include/bits/stat.h /usr/include/bits/stdio_lim.h
ucd-snmp/loadave.o: /usr/include/bits/time.h /usr/include/bits/types.h
ucd-snmp/loadave.o: /usr/include/bits/waitflags.h
ucd-snmp/loadave.o: /usr/include/bits/waitstatus.h
ucd-snmp/loadave.o: /usr/include/bits/wordsize.h /usr/include/endian.h
ucd-snmp/loadave.o: /usr/include/errno.h /usr/include/fcntl.h
ucd-snmp/loadave.o: /usr/include/features.h /usr/include/_G_config.h
ucd-snmp/loadave.o: /usr/include/gconv.h /usr/include/gnu/stubs.h
ucd-snmp/loadave.o: /usr/include/libio.h /usr/include/linux/errno.h
ucd-snmp/loadave.o: /usr/include/linux/limits.h /usr/include/linux/param.h
ucd-snmp/loadave.o: /usr/include/malloc.h /usr/include/netinet/in.h
ucd-snmp/loadave.o: /usr/include/signal.h /usr/include/stdint.h
ucd-snmp/loadave.o: /usr/include/stdio.h /usr/include/stdlib.h
ucd-snmp/loadave.o: /usr/include/string.h /usr/include/sys/cdefs.h
ucd-snmp/loadave.o: /usr/include/syslog.h /usr/include/sys/param.h
ucd-snmp/loadave.o: /usr/include/sys/select.h /usr/include/sys/socket.h
ucd-snmp/loadave.o: /usr/include/sys/stat.h /usr/include/sys/syslog.h
ucd-snmp/loadave.o: /usr/include/sys/sysmacros.h /usr/include/sys/time.h
ucd-snmp/loadave.o: /usr/include/sys/types.h /usr/include/sys/ucontext.h
ucd-snmp/loadave.o: /usr/include/time.h /usr/include/ucontext.h
ucd-snmp/loadave.o: /usr/include/unistd.h /usr/include/wchar.h
ucd-snmp/loadave.o: /usr/include/xlocale.h
ucd-snmp/loadave.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
ucd-snmp/loadave.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
ucd-snmp/loadave.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
ucd-snmp/loadave.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
ucd-snmp/loadave.o: util_funcs.h
ucd-snmp/pass.o: mibdefs.h mibincl.h struct.h ucd-snmp/extensible.h
ucd-snmp/pass.o: ucd-snmp/pass.h /usr/include/alloca.h
ucd-snmp/pass.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
ucd-snmp/pass.o: /usr/include/bits/byteswap.h /usr/include/bits/confname.h
ucd-snmp/pass.o: /usr/include/bits/endian.h /usr/include/bits/environments.h
ucd-snmp/pass.o: /usr/include/bits/in.h /usr/include/bits/posix_opt.h
ucd-snmp/pass.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
ucd-snmp/pass.o: /usr/include/bits/select.h /usr/include/bits/siginfo.h
ucd-snmp/pass.o: /usr/include/bits/sigset.h /usr/include/bits/sockaddr.h
ucd-snmp/pass.o: /usr/include/bits/socket.h /usr/include/bits/stdio_lim.h
ucd-snmp/pass.o: /usr/include/bits/time.h /usr/include/bits/types.h
ucd-snmp/pass.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
ucd-snmp/pass.o: /usr/include/bits/wordsize.h /usr/include/ctype.h
ucd-snmp/pass.o: /usr/include/endian.h /usr/include/features.h
ucd-snmp/pass.o: /usr/include/_G_config.h /usr/include/gconv.h
ucd-snmp/pass.o: /usr/include/gnu/stubs.h /usr/include/libio.h
ucd-snmp/pass.o: /usr/include/malloc.h /usr/include/netinet/in.h
ucd-snmp/pass.o: /usr/include/stdint.h /usr/include/stdio.h
ucd-snmp/pass.o: /usr/include/stdlib.h /usr/include/string.h
ucd-snmp/pass.o: /usr/include/sys/cdefs.h /usr/include/syslog.h
ucd-snmp/pass.o: /usr/include/sys/select.h /usr/include/sys/socket.h
ucd-snmp/pass.o: /usr/include/sys/syslog.h /usr/include/sys/sysmacros.h
ucd-snmp/pass.o: /usr/include/sys/time.h /usr/include/sys/types.h
ucd-snmp/pass.o: /usr/include/sys/wait.h /usr/include/time.h
ucd-snmp/pass.o: /usr/include/unistd.h /usr/include/wchar.h
ucd-snmp/pass.o: /usr/include/xlocale.h
ucd-snmp/pass.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
ucd-snmp/pass.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
ucd-snmp/pass.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
ucd-snmp/pass.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
ucd-snmp/pass.o: util_funcs.h
ucd-snmp/pass_persist.o: mibdefs.h mibincl.h struct.h ucd-snmp/extensible.h
ucd-snmp/pass_persist.o: ucd-snmp/pass_persist.h /usr/include/alloca.h
ucd-snmp/pass_persist.o: /usr/include/asm/errno.h
ucd-snmp/pass_persist.o: /usr/include/asm/sigcontext.h
ucd-snmp/pass_persist.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
ucd-snmp/pass_persist.o: /usr/include/bits/byteswap.h
ucd-snmp/pass_persist.o: /usr/include/bits/confname.h
ucd-snmp/pass_persist.o: /usr/include/bits/endian.h
ucd-snmp/pass_persist.o: /usr/include/bits/environments.h
ucd-snmp/pass_persist.o: /usr/include/bits/errno.h /usr/include/bits/in.h
ucd-snmp/pass_persist.o: /usr/include/bits/posix_opt.h
ucd-snmp/pass_persist.o: /usr/include/bits/pthreadtypes.h
ucd-snmp/pass_persist.o: /usr/include/bits/sched.h /usr/include/bits/select.h
ucd-snmp/pass_persist.o: /usr/include/bits/sigaction.h
ucd-snmp/pass_persist.o: /usr/include/bits/sigcontext.h
ucd-snmp/pass_persist.o: /usr/include/bits/siginfo.h
ucd-snmp/pass_persist.o: /usr/include/bits/signum.h /usr/include/bits/sigset.h
ucd-snmp/pass_persist.o: /usr/include/bits/sigstack.h
ucd-snmp/pass_persist.o: /usr/include/bits/sigthread.h
ucd-snmp/pass_persist.o: /usr/include/bits/sockaddr.h
ucd-snmp/pass_persist.o: /usr/include/bits/socket.h
ucd-snmp/pass_persist.o: /usr/include/bits/stdio_lim.h
ucd-snmp/pass_persist.o: /usr/include/bits/time.h /usr/include/bits/types.h
ucd-snmp/pass_persist.o: /usr/include/bits/waitflags.h
ucd-snmp/pass_persist.o: /usr/include/bits/waitstatus.h
ucd-snmp/pass_persist.o: /usr/include/bits/wordsize.h /usr/include/ctype.h
ucd-snmp/pass_persist.o: /usr/include/endian.h /usr/include/errno.h
ucd-snmp/pass_persist.o: /usr/include/features.h /usr/include/_G_config.h
ucd-snmp/pass_persist.o: /usr/include/gconv.h /usr/include/gnu/stubs.h
ucd-snmp/pass_persist.o: /usr/include/libio.h /usr/include/linux/errno.h
ucd-snmp/pass_persist.o: /usr/include/malloc.h /usr/include/netinet/in.h
ucd-snmp/pass_persist.o: /usr/include/signal.h /usr/include/stdint.h
ucd-snmp/pass_persist.o: /usr/include/stdio.h /usr/include/stdlib.h
ucd-snmp/pass_persist.o: /usr/include/string.h /usr/include/sys/cdefs.h
ucd-snmp/pass_persist.o: /usr/include/syslog.h /usr/include/sys/select.h
ucd-snmp/pass_persist.o: /usr/include/sys/socket.h /usr/include/sys/syslog.h
ucd-snmp/pass_persist.o: /usr/include/sys/sysmacros.h /usr/include/sys/time.h
ucd-snmp/pass_persist.o: /usr/include/sys/types.h /usr/include/sys/ucontext.h
ucd-snmp/pass_persist.o: /usr/include/sys/wait.h /usr/include/time.h
ucd-snmp/pass_persist.o: /usr/include/ucontext.h /usr/include/unistd.h
ucd-snmp/pass_persist.o: /usr/include/wchar.h /usr/include/xlocale.h
ucd-snmp/pass_persist.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
ucd-snmp/pass_persist.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
ucd-snmp/pass_persist.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
ucd-snmp/pass_persist.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
ucd-snmp/pass_persist.o: util_funcs.h
ucd-snmp/proc.o: mibdefs.h mibincl.h struct.h ucd-snmp/errormib.h
ucd-snmp/proc.o: ucd-snmp/proc.h /usr/include/alloca.h
ucd-snmp/proc.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
ucd-snmp/proc.o: /usr/include/bits/byteswap.h /usr/include/bits/confname.h
ucd-snmp/proc.o: /usr/include/bits/endian.h /usr/include/bits/environments.h
ucd-snmp/proc.o: /usr/include/bits/huge_val.h /usr/include/bits/in.h
ucd-snmp/proc.o: /usr/include/bits/mathcalls.h /usr/include/bits/mathdef.h
ucd-snmp/proc.o: /usr/include/bits/nan.h /usr/include/bits/posix_opt.h
ucd-snmp/proc.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
ucd-snmp/proc.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
ucd-snmp/proc.o: /usr/include/bits/sockaddr.h /usr/include/bits/socket.h
ucd-snmp/proc.o: /usr/include/bits/stdio_lim.h /usr/include/bits/time.h
ucd-snmp/proc.o: /usr/include/bits/types.h /usr/include/bits/waitflags.h
ucd-snmp/proc.o: /usr/include/bits/waitstatus.h /usr/include/bits/wordsize.h
ucd-snmp/proc.o: /usr/include/ctype.h /usr/include/endian.h
ucd-snmp/proc.o: /usr/include/features.h /usr/include/_G_config.h
ucd-snmp/proc.o: /usr/include/gconv.h /usr/include/gnu/stubs.h
ucd-snmp/proc.o: /usr/include/libio.h /usr/include/malloc.h
ucd-snmp/proc.o: /usr/include/math.h /usr/include/netinet/in.h
ucd-snmp/proc.o: /usr/include/stdint.h /usr/include/stdio.h
ucd-snmp/proc.o: /usr/include/stdlib.h /usr/include/string.h
ucd-snmp/proc.o: /usr/include/sys/cdefs.h /usr/include/syslog.h
ucd-snmp/proc.o: /usr/include/sys/select.h /usr/include/sys/socket.h
ucd-snmp/proc.o: /usr/include/sys/syslog.h /usr/include/sys/sysmacros.h
ucd-snmp/proc.o: /usr/include/sys/time.h /usr/include/sys/types.h
ucd-snmp/proc.o: /usr/include/time.h /usr/include/unistd.h
ucd-snmp/proc.o: /usr/include/wchar.h /usr/include/xlocale.h
ucd-snmp/proc.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/float.h
ucd-snmp/proc.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
ucd-snmp/proc.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
ucd-snmp/proc.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
ucd-snmp/proc.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
ucd-snmp/proc.o: util_funcs.h
ucd-snmp/registry.o: mibincl.h ucd-snmp/registry.h /usr/include/alloca.h
ucd-snmp/registry.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
ucd-snmp/registry.o: /usr/include/bits/byteswap.h /usr/include/bits/endian.h
ucd-snmp/registry.o: /usr/include/bits/in.h /usr/include/bits/pthreadtypes.h
ucd-snmp/registry.o: /usr/include/bits/sched.h /usr/include/bits/select.h
ucd-snmp/registry.o: /usr/include/bits/sigset.h /usr/include/bits/sockaddr.h
ucd-snmp/registry.o: /usr/include/bits/socket.h /usr/include/bits/stdio_lim.h
ucd-snmp/registry.o: /usr/include/bits/time.h /usr/include/bits/types.h
ucd-snmp/registry.o: /usr/include/bits/waitflags.h
ucd-snmp/registry.o: /usr/include/bits/waitstatus.h
ucd-snmp/registry.o: /usr/include/bits/wordsize.h /usr/include/endian.h
ucd-snmp/registry.o: /usr/include/features.h /usr/include/_G_config.h
ucd-snmp/registry.o: /usr/include/gconv.h /usr/include/gnu/stubs.h
ucd-snmp/registry.o: /usr/include/libio.h /usr/include/malloc.h
ucd-snmp/registry.o: /usr/include/netinet/in.h /usr/include/stdint.h
ucd-snmp/registry.o: /usr/include/stdio.h /usr/include/stdlib.h
ucd-snmp/registry.o: /usr/include/sys/cdefs.h /usr/include/syslog.h
ucd-snmp/registry.o: /usr/include/sys/select.h /usr/include/sys/socket.h
ucd-snmp/registry.o: /usr/include/sys/syslog.h /usr/include/sys/sysmacros.h
ucd-snmp/registry.o: /usr/include/sys/time.h /usr/include/sys/types.h
ucd-snmp/registry.o: /usr/include/time.h /usr/include/wchar.h
ucd-snmp/registry.o: /usr/include/xlocale.h
ucd-snmp/registry.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
ucd-snmp/registry.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
ucd-snmp/registry.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
ucd-snmp/registry.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
ucd-snmp/versioninfo.o: mibdefs.h mibincl.h struct.h ucd-snmp/versioninfo.h
ucd-snmp/versioninfo.o: /usr/include/alloca.h /usr/include/asm/socket.h
ucd-snmp/versioninfo.o: /usr/include/asm/sockios.h
ucd-snmp/versioninfo.o: /usr/include/bits/byteswap.h
ucd-snmp/versioninfo.o: /usr/include/bits/endian.h /usr/include/bits/in.h
ucd-snmp/versioninfo.o: /usr/include/bits/pthreadtypes.h
ucd-snmp/versioninfo.o: /usr/include/bits/sched.h /usr/include/bits/select.h
ucd-snmp/versioninfo.o: /usr/include/bits/sigset.h
ucd-snmp/versioninfo.o: /usr/include/bits/sockaddr.h
ucd-snmp/versioninfo.o: /usr/include/bits/socket.h
ucd-snmp/versioninfo.o: /usr/include/bits/stdio_lim.h /usr/include/bits/time.h
ucd-snmp/versioninfo.o: /usr/include/bits/types.h
ucd-snmp/versioninfo.o: /usr/include/bits/waitflags.h
ucd-snmp/versioninfo.o: /usr/include/bits/waitstatus.h
ucd-snmp/versioninfo.o: /usr/include/bits/wordsize.h /usr/include/endian.h
ucd-snmp/versioninfo.o: /usr/include/features.h /usr/include/_G_config.h
ucd-snmp/versioninfo.o: /usr/include/gconv.h /usr/include/gnu/stubs.h
ucd-snmp/versioninfo.o: /usr/include/libio.h /usr/include/malloc.h
ucd-snmp/versioninfo.o: /usr/include/netinet/in.h /usr/include/stdint.h
ucd-snmp/versioninfo.o: /usr/include/stdio.h /usr/include/stdlib.h
ucd-snmp/versioninfo.o: /usr/include/sys/cdefs.h /usr/include/syslog.h
ucd-snmp/versioninfo.o: /usr/include/sys/select.h /usr/include/sys/socket.h
ucd-snmp/versioninfo.o: /usr/include/sys/syslog.h /usr/include/sys/sysmacros.h
ucd-snmp/versioninfo.o: /usr/include/sys/time.h /usr/include/sys/types.h
ucd-snmp/versioninfo.o: /usr/include/time.h /usr/include/wchar.h
ucd-snmp/versioninfo.o: /usr/include/xlocale.h
ucd-snmp/versioninfo.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
ucd-snmp/versioninfo.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
ucd-snmp/versioninfo.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
ucd-snmp/versioninfo.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
ucd-snmp/versioninfo.o: util_funcs.h
util_funcs.o: ../agent_read_config.h ../agent_registry.h ../agent_trap.h
util_funcs.o: ../../config.h ../../m/generic.h mibdefs.h mibincl.h
util_funcs.o: ../../mib_module_config.h ../../s/generic.h ../../s/linux.h
util_funcs.o: ../../snmplib/asn1.h ../../snmplib/callback.h
util_funcs.o: ../../snmplib/getopt.h ../../snmplib/mib.h
util_funcs.o: ../../snmplib/read_config.h ../../snmplib/snmp_api.h
util_funcs.o: ../../snmplib/snmp_debug.h ../../snmplib/snmp.h
util_funcs.o: ../../snmplib/snmp_impl.h ../../snmplib/snmp_logging.h
util_funcs.o: ../../snmplib/system.h ../../snmplib/tools.h ../snmp_vars.h
util_funcs.o: ../../s/sysv.h struct.h ucd-snmp/errormib.h
util_funcs.o: /usr/include/alloca.h /usr/include/asm/errno.h
util_funcs.o: /usr/include/asm/sigcontext.h /usr/include/asm/socket.h
util_funcs.o: /usr/include/asm/sockios.h /usr/include/bits/byteswap.h
util_funcs.o: /usr/include/bits/confname.h /usr/include/bits/endian.h
util_funcs.o: /usr/include/bits/environments.h /usr/include/bits/errno.h
util_funcs.o: /usr/include/bits/fcntl.h /usr/include/bits/in.h
util_funcs.o: /usr/include/bits/posix_opt.h /usr/include/bits/pthreadtypes.h
util_funcs.o: /usr/include/bits/sched.h /usr/include/bits/select.h
util_funcs.o: /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h
util_funcs.o: /usr/include/bits/siginfo.h /usr/include/bits/signum.h
util_funcs.o: /usr/include/bits/sigset.h /usr/include/bits/sigstack.h
util_funcs.o: /usr/include/bits/sigthread.h /usr/include/bits/sockaddr.h
util_funcs.o: /usr/include/bits/socket.h /usr/include/bits/stat.h
util_funcs.o: /usr/include/bits/stdio_lim.h /usr/include/bits/time.h
util_funcs.o: /usr/include/bits/types.h /usr/include/bits/waitflags.h
util_funcs.o: /usr/include/bits/waitstatus.h /usr/include/bits/wordsize.h
util_funcs.o: /usr/include/ctype.h /usr/include/endian.h /usr/include/errno.h
util_funcs.o: /usr/include/fcntl.h /usr/include/features.h
util_funcs.o: /usr/include/_G_config.h /usr/include/gconv.h
util_funcs.o: /usr/include/gnu/stubs.h /usr/include/libio.h
util_funcs.o: /usr/include/linux/errno.h /usr/include/malloc.h
util_funcs.o: /usr/include/netinet/in.h /usr/include/signal.h
util_funcs.o: /usr/include/stdint.h /usr/include/stdio.h /usr/include/stdlib.h
util_funcs.o: /usr/include/string.h /usr/include/sys/cdefs.h
util_funcs.o: /usr/include/syslog.h /usr/include/sys/select.h
util_funcs.o: /usr/include/sys/socket.h /usr/include/sys/stat.h
util_funcs.o: /usr/include/sys/syslog.h /usr/include/sys/sysmacros.h
util_funcs.o: /usr/include/sys/time.h /usr/include/sys/types.h
util_funcs.o: /usr/include/sys/ucontext.h /usr/include/sys/wait.h
util_funcs.o: /usr/include/time.h /usr/include/ucontext.h
util_funcs.o: /usr/include/unistd.h /usr/include/wchar.h
util_funcs.o: /usr/include/xlocale.h
util_funcs.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
util_funcs.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
util_funcs.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
util_funcs.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
util_funcs.o: util_funcs.h ../var_struct.h
