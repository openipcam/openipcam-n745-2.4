# Generated automatically from Makefile.in by configure.
#
# Minimum environment and virtual path setup
#
SHELL		= /bin/sh
srcdir		= .
top_srcdir	= ..


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
CFLAGS		= -g -O2 -Dlinux
EXTRACPPFLAGS	= -x c
LDFLAGS		=  

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

SCRIPTS=snmpcheck tkmib
SCRIPTSMADEFORPERL=snmpcheck.made tkmib.made mib2c.made fixproc.made \
	ipf-mod.pl.made rsnmp.made
PERLPROG=/usr/bin/perl

all:
	@if test "x$(PERLPROG)" != "x"; then \
	  $(MAKE) $(SCRIPTSMADEFORPERL); \
	fi

snmpcheck: snmpcheck.def ../sedscript
	$(SED) -f ../sedscript $(srcdir)/snmpcheck.def > snmpcheck
	chmod a+x snmpcheck

clean:
	rm -f snmpcheck $(SCRIPTSMADEFORPERL)

snmpcheck.made: snmpcheck
	touch snmpcheck.made
	$(PERLPROG) -i -p -e 's@^#!.*/perl@#!$(PERLPROG)@' snmpcheck

tkmib.made: tkmib
	touch tkmib.made
	@if test ! -f tkmib; then\
	  cp ${srcdir}/tkmib .; \
	fi
	$(PERLPROG) -i -p -e 's@^#!.*/perl@#!$(PERLPROG)@' tkmib

mib2c.made: mib2c
	touch mib2c.made
	@if test ! -f mib2c; then\
	  cp ${srcdir}/mib2c .; \
	fi
	$(PERLPROG) -i -p -e 's@^#!.*/perl@#!$(PERLPROG)@' mib2c

ipf-mod.pl.made: ipf-mod.pl
	touch ipf-mod.pl.made
	@if test ! -f ipf-mod.pl; then\
	  cp ${srcdir}/ipf-mod.pl .; \
	fi
	$(PERLPROG) -i -p -e 's@^#!.*/perl@#!$(PERLPROG)@' ipf-mod.pl

fixproc.made: fixproc
	touch fixproc.made
	@if test ! -f fixproc; then\
	  cp ${srcdir}/fixproc .; \
	fi
	$(PERLPROG) -i -p -e 's@^#!.*/perl@#!$(PERLPROG)@' fixproc

rsnmp.made: rsnmp
	touch rsnmp.made
	@if test ! -f rsnmp; then\
	  cp ${srcdir}/rsnmp .; \
	fi
	$(PERLPROG) -i -p -e 's@^#!.*/perl@#!$(PERLPROG)@' rsnmp

install:
	@if test "x$(PERLPROG)" != "x"; then \
	  for i in $(SCRIPTS) ; do $(INSTALL) $$i $(bindir) ; echo "install:  installed $$i in $(bindir)" ; done \
	fi

