# $Id: makefile,v 1.1.1.1 2006-07-11 09:32:58 andy Exp $
#

srcdir		= .

#CC		= gcc
INSTALL		= /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA	= ${INSTALL} -m 644
SHELL		= /bin/sh

# Where to install
#prefix		= /usr/squid
#exec_prefix	= ${prefix}
prefix		= /usr/squid
exec_prefix	= /bin

SUBDIRS		= lib  scripts src icons errors 
AUTHDIRS	= auth_modules/NCSA \
		  auth_modules/getpwnam \
		  auth_modules/PAM \
		  auth_modules/SMB \
		  auth_modules/LDAP

noargs: all

all install:
	@for dir in $(SUBDIRS); do \
		echo "Making $@ in $$dir..."; \
		(cd $$dir ; $(MAKE) $(MFLAGS) prefix="$(prefix)" $@) || exit 1; \
	done

clean:
	@for dir in $(SUBDIRS); do \
		echo "Making $@ in $$dir..."; \
		(cd $$dir ; $(MAKE) $(MFLAGS) prefix="$(prefix)" $@); \
	done


install-pinger:
	(cd src; $(MAKE) $(MFLAGS) prefix="$(prefix)" $@)

almostclean: clean
	rm -f config.log makefile
	rm -f include/paths.h include/autoconf.h include/config.h
	rm -f auth_modules/dummy
	@for dir in $(SUBDIRS) $(AUTHDIRS) contrib; do \
		echo Making distclean in $$dir; \
		(cd $$dir; $(MAKE) $(MFLAGS) prefix="$(prefix)" distclean); \
	done

distclean: almostclean
	rm -f config.status config.cache

targz: distclean
	tar cvf current.tar . ; gzip current.tar

#makefile: configure config.status makefile.in lib/Makefile.in include/config.h.in src/Makefile.in include/version.h
#	@echo "Package configuration updated. Cleaning and reconfiguring" ;\
#	./config.status --recheck;\
#	./config.status ;\
#	$(MAKE) $(MFLAGS) clean

depend:

romfs:
	[ -d $(ROMFSDIR)$(prefix) ] || mkdir -p $(ROMFSDIR)$(prefix)
	[ -d $(ROMFSDIR)$(prefix)/bin ] || mkdir $(ROMFSDIR)$(prefix)/bin
	[ -d $(ROMFSDIR)$(prefix)/etc ] || mkdir $(ROMFSDIR)$(prefix)/etc
	[ -d $(ROMFSDIR)$(prefix)/icons ] || mkdir $(ROMFSDIR)$(prefix)/icons
	[ -d $(ROMFSDIR)$(prefix)/errors ] || mkdir $(ROMFSDIR)$(prefix)/errors
	$(ROMFSINST) -s /mnt/hda/squid/logs $(prefix)/logs
	$(ROMFSINST) -s /mnt/hda/squid/cache $(prefix)/cache
	$(ROMFSINST) src/squid $(prefix)/bin/squid
	$(ROMFSINST) src/client $(prefix)/bin/client
	$(ROMFSINST) src/dnsserver $(prefix)/bin/dnsserver
	$(ROMFSINST) src/unlinkd $(prefix)/bin/unlinkd
	$(ROMFSINST) src/squid.conf $(prefix)/etc/squid.conf
	$(ROMFSINST) src/squid.conf $(prefix)/etc/squid.conf.default
	$(ROMFSINST) src/mime.conf $(prefix)/etc/mime.conf
	$(ROMFSINST) src/mime.conf $(prefix)/etc/mime.conf.default
	cd errors/English; \
	for f in ERR_*; do \
		$(ROMFSINST) $$f $(prefix)/errors/$$f; \
        done
