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

#
# Makefile for snmpd
#
DLMODDIR=	$(libdir)/snmp/dlmod
TARG=	 dlmod_mib.so example.so
USELIBS= $(top_srcdir)/snmplib/libsnmp.$(SHLIB_EXTENSION)$(SHLIB_VERSION)
LIBS=#	 -L$(top_srcdir)/snmplib $(CC_RUNTIME_ARG) -lsnmp -lcrypto -ldb1 -lz -lm -lelf 

CPPFLAGS= -I$(top_srcdir) -I$(top_srcdir)/snmplib \
	-I.  -I.. -I../.. -I$(top_srcdir)/agent/mibgroup  -I$(top_srcdir)/agent/mibgroup/misc $(INCS)

# CC = gcc $(CPPFLAGS)


all: $(TARG)

DLMOD_MIB_OBJS= dlmod_mib.o
EXAMPLE_OBJS= example.o

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SHLIB_CFLAGS) -o $@ -c $<

dlmod_mib.so: $(DLMOD_MIB_OBJS)
	$(SHLIB_LD_CMD) $@ $(DLMOD_MIB_OBJS) $(LIBS)

example.so: $(EXAMPLE_OBJS)
	$(SHLIB_LD_CMD) $@ $(EXAMPLE_OBJS) $(LIBS)

clean:
	rm -f *.o ${TARG}

install:  all installdirs
	@$(INSTALL) $(TARG) $(DLMODDIR) ; echo "install:  installed $(TARG) in $(DLMODDIR) "

installdirs:
	@$(SHELL) $(top_srcdir)/mkinstalldirs $(DLMODDIR)

dependlocal: 
	makedepend $(CPPFLAGS) $(srcdir)/*.c

depend: dependlocal

nosysdepend:
	makedepend $(CPPFLAGS) *.c
	perl -n -i.bak $(top_srcdir)/makenosysdepend.pl Makefile

makefileindepend: nosysdepend
	perl $(top_srcdir)/makefileindepend.pl


# DO NOT DELETE THIS LINE -- make depend depends on it.
