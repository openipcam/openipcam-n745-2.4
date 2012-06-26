# Generated automatically from Makefile.in by configure.
#
#  This file is a Makefile for compiling and installing Cache Manager. 
#  Cache Manager is a manager program for Internet Object Cache.
#

prefix		= /usr/squid
#exec_prefix     = ${prefix}
top_srcdir      = ..
#bindir          = ${exec_prefix}/bin
#sysconfdir      = ${prefix}/etc
srcdir          = .

sysconfdir	= /etc/config
realsysconfdir	= /etc/default
bindir		= /bin
exec_prefix	= /

CC		= gcc
INSTALL		= /usr/bin/install -c
INSTALL_BIN 	= ${INSTALL}
INSTALL_FILE 	= ${INSTALL} -m 644
RANLIB		= ranlib
LN_S		= ln -s

all: 

install:	
	@if test ! -d $(ROMFSDIR)/$(exec_prefix); then \
		echo "mkdir $(exec_prefix)"; \
		mkdir $(ROMFSDIR)/$(exec_prefix); \
	fi
	@if test ! -d $(ROMFSDIR)/$(bindir); then \
		echo "mkdir $(bindir)"; \
		mkdir $(ROMFSDIR)/$(bindir); \
	fi
	$(INSTALL_BIN) RunCache $(ROMFSDIR)/$(bindir)
	$(INSTALL_BIN) RunAccel $(ROMFSDIR)/$(bindir)

clean:

distclean:
	-rm -f RunCache RunAccel Makefile
