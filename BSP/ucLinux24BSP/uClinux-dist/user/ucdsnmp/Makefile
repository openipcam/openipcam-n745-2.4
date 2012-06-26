# Generated automatically from Makefile.top:Makefile.in by configure.
#
# Minimum environment and virtual path setup
#
SHELL		= /bin/sh
srcdir		= .
top_srcdir	= .


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
# RANLIB		= ranlib
LN_S		= ln -s

#
# Compiler arguments
#
CFLAGS		+= -g
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
# Makefile.in (at the root of ucd-snmp)
#

TARG		=	bin/snmpget bin/snmpgetnext bin/snmpset 	\
			bin/snmptranslate bin/snmpwalk bin/snmpbulkwalk \
			bin/snmptest bin/snmptrapd bin/snmpnetstat bin/snmpd

SUBDIRS		= snmplib agent
# SUBDIRS		= snmplib agent apps local ov man
INSTALLDIRS	= snmplib agent
TESTDIRS	= testing

CPP		= $(CC) -E 						\
			-I$(srcdir)/agent/mibgroup -I. -I$(srcdir)	\
			-DDONT_INC_STRUCTS -DBINDIR=$(bindir) 		\
			$(EXTRACPPFLAGS) $(INCS)


CPP = $(CC) -E -I$(srcdir)/agent/mibgroup -I. -I$(srcdir) -DDONT_INC_STRUCTS -DBINDIR=$(bindir) $(EXTRACPPFLAGS) $(INCS)

INSTALLHEADERS=ucd-snmp-config.h $(srcdir)/version.h

all:	sedscript EXAMPLE.conf ucd-snmp-config.h subdirs

test:   all
	( cd testing; $(MAKE) test )

sedscript: sedscript.in config.h $(srcdir)/agent/mibgroup/mibdefs.h
	$(CPP) -DPREFIX=$(prefix) -DLIBDIR=$(libdir) -DDATADIR=$(datadir) $(srcdir)/sedscript.in | egrep '^s[/#]' | sed 's/REMOVEME//g;s# */#/#g;s/ *#/#/g;s#/ *#/#g;s/# g/#g/;' > sedscript


ucd-snmp-config.h: config.h
	@egrep -v "IN_UCD_SNMP_SOURCE|SYSTEM_INCLUDE|MACHINE_INCLUDE|mib_module_config" config.h > ucd-snmp-config.h

subdirs:
	for i in $(SUBDIRS) ; do	\
        ( cd $$i ; $(MAKE) ) ;		\
	done


test: all testdirs

testdirs:
	for i in $(TESTDIRS) ; do	\
        ( cd $$i ; $(MAKE) ) ;		\
	done


distall: ${srcdir}/configure ${srcdir}/config.h 


install:    all installdirs
	for i in $(INSTALLDIRS) ; do	\
        ( cd $$i ; $(MAKE) install ) ;	\
	done
	@for i in $(INSTALLHEADERS) ; do \
		$(INSTALL) $$i $(includedir) ; \
		echo "install:  installed $$i in $(includedir)";\
	done

installdirs:
	@$(SHELL) $(srcdir)/mkinstalldirs $(snmplibdir) $(mibdir) $(includedir)
	@-$(SHELL) $(srcdir)/mkinstalldirs $(persistentdir)

apps/snmpget apps/snmpgetnext apps/snmpset apps/snmptranslate apps/snmpwalk apps/snmpbulkwalk apps/snmptest apps/snmptrapd apps/snmpnetstat/snmpnetstat agent/snmpd: makeall


depend:
	cd snmplib; $(MAKE) depend
	cd apps; $(MAKE) depend
	cd agent; $(MAKE) depend
	cd testing; $(MAKE) depend

nosysdepend:
	cd snmplib; $(MAKE) nosysdepend
	cd apps; $(MAKE) nosysdepend
	cd agent; $(MAKE) nosysdepend
	cd testing; $(MAKE) nosysdepend

makefileindepend:
	cd snmplib; $(MAKE) makefileindepend
	cd apps; $(MAKE) makefileindepend
	cd agent; $(MAKE) makefileindepend
	cd testing; $(MAKE) makefileindepend


clean:
	for i in $(SUBDIRS) $(TESTDIRS); do \
        ( cd $$i ; $(MAKE) clean ) ; \
	done
	rm -f EXAMPLE.conf sedscript ucd-snmp.txt config.h

distclean: clean configclean

configclean:
	rm -f config.cache config.status config.log config.h
	rm -f Makefile snmplib/Makefile				\
		agent/Makefile agent/mibgroup/Makefile		\
		apps/Makefile  apps/snmpnetstat/Makefile	\
		man/Makefile mibs/Makefile ov/Makefile		\
		local/Makefile testing/Makefile
	rm -f agent/dlmods/Makefile
	rm -f mibs/.index
	rm -f mib_module_config.h ucd-snmp-config.h 	\
		agent/mibgroup/mib_module_includes.h 	\
		agent/mibgroup/mib_module_inits.h 	\
		agent/mibgroup/mib_module_shutdown.h 	\
		agent/mibgroup/mib_module_dot_conf.h
	rm -f *.core


configure: configure.in aclocal.m4
	cd ${srcdir} && autoconf
	echo "Please run configure now."
	sh -c exit 2


# autoheader might not change config.h.in, so touch a stamp file.
#
config.h.in: stamp-h.in
stamp-h.in: configure.in acconfig.h
	cd ${srcdir} && autoheader
	echo timestamp > ${srcdir}/stamp-h.in

config.h: stamp-h
	./mkconfigh $(ROOTDIR)/lib/$(LIBCDIR)

stamp-h: config.h.in config.status
	CONFIG_FILES= ./config.status
	echo timestamp > ${srcdir}/stamp-h

touchit:
	touch configure config.h.in
	touch config.h config.status
	touch stamp-h stamp-h.in
	touch Makefile

Makefile: Makefile.in config.status
	CONFIG_HEADERS= ./config.status

config.status: configure
	./config.status --recheck

EXAMPLE.conf: sedscript EXAMPLE.conf.def
	$(SED) -f sedscript $(srcdir)/EXAMPLE.conf.def > EXAMPLE.conf

TAGS:
	find $(srcdir) -name '*.[ch]' -print | etags -

version:
	@if test "x$(VERSION)" = "x"; then \
	  echo "you need to supply a VERSION string."; \
	  exit 2; \
	fi
	agent/mibgroup/versiontag $(VERSION) reverse

tag:
	agent/mibgroup/versiontag $(VERSION) tag

tar:
	agent/mibgroup/versiontag $(VERSION) tar

dist: version tag tar

FAQ.html:
	local/FAQ2HTML FAQ

# If we're installing the UCD SMNP daemon, add it to /etc/inittab.  We also
# must provide a default configuration file.
romfs:
	for i in $(SUBDIRS) ; do \
        $(MAKE) -C $$i romfs || exit $?; \
	done
	$(ROMFSINST) ucdsnmpd-default /etc/default/snmpd.conf
	$(ROMFSINST) ucdsnmp-default  /etc/default/snmp.conf
