# Generated automatically from Makefile.in by configure.
#
#  Makefile for the Squid Object Cache server
#
#  $Id: Makefile,v 1.1.1.1 2006-07-11 09:32:59 andy Exp $
#
#  Uncomment and customize the following to suit your needs:
#

prefix		= /usr/squid
exec_prefix	= ${prefix}
exec_suffix	= 
cgi_suffix	= .cgi
top_srcdir	= ../..
bindir		= ${exec_prefix}/bin
libexecdir      = ${bindir}
sysconfdir	= ${prefix}/etc
localstatedir   = ${prefix}
srcdir		= .

# Gotta love the DOS legacy
#
NCSA_AUTH_EXE	= ncsa_auth$(exec_suffix)

DEFAULT_PASSWD_FILE     = $(sysconfdir)/passwd

CC		= gcc
MAKEDEPEND	= /usr/bin/makedepend
INSTALL		= /usr/bin/install -c
INSTALL_BIN 	= ${INSTALL}
INSTALL_FILE 	= ${INSTALL} -m 644
INSTALL_SUID	= ${INSTALL} -o root -m 4755
RANLIB		= ranlib
LN_S		= ln -s
PERL            = /usr/bin/perl
CRYPTLIB	= -lcrypt
REGEXLIB	= -lregex
PTHREADLIB	= 
SNMPLIB		= 
MALLOCLIB	= 
AC_CFLAGS	= -g -O2 -Wall
LDFLAGS		= -g
XTRA_LIBS	= 
XTRA_OBJS 	= 
MV		= /bin/mv
RM		= /bin/rm
SHELL		= /bin/sh


INCLUDE		= -I. -I../../include -I$(top_srcdir)/include
CFLAGS 		= $(AC_CFLAGS) $(INCLUDE) $(DEFINES)
AUTH_LIBS	= -L../../lib -lmiscutil $(CRYPTLIB) $(XTRA_LIBS)

PROGS           = $(NCSA_AUTH_EXE)
OBJS	 	= ncsa_auth.o

all:    $(NCSA_AUTH_EXE)

$(OBJS): $(top_srcdir)/include/version.h

$(NCSA_AUTH_EXE): ncsa_auth.o
	$(CC) $(LDFLAGS) ncsa_auth.o -o $@ $(AUTH_LIBS)

install-mkdirs:
	-@if test ! -d $(prefix); then \
		echo "mkdir $(prefix)"; \
		mkdir $(prefix); \
	fi
	-@if test ! -d $(bindir); then \
		echo "mkdir $(bindir)"; \
		mkdir $(bindir); \
	fi

# Michael Lupp <mike@nemesis.saar.de> wants to know about additions
# to the install target.
install: all install-mkdirs
	@for f in $(PROGS); do \
		if test -f $(bindir)/$$f; then \
			echo $(MV) $(bindir)/$$f $(bindir)/-$$f; \
			$(MV) $(bindir)/$$f $(bindir)/-$$f; \
		fi; \
		echo $(INSTALL_BIN) $$f $(bindir); \
		$(INSTALL_BIN) $$f $(bindir); \
		if test -f $(bindir)/-$$f; then \
			echo $(RM) -f $(bindir)/-$$f; \
			$(RM) -f $(bindir)/-$$f; \
		fi; \
	done

clean: 
	-rm -rf *.o *pure_* core $(PROGS)

distclean:	clean
	-rm -f Makefile

tags:
	ctags *.[ch] ../include/*.h ../lib/*.[ch]

depend:
	$(MAKEDEPEND) -I../include -I. -fMakefile *.c
