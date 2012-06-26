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
PAM_AUTH_EXE	= pam_auth$(exec_suffix)

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
DEFINES		= 

INCLUDE		= -I. -I../../include -I$(top_srcdir)/include
CFLAGS 		= $(AC_CFLAGS) $(INCLUDE) $(DEFINES)
AUTH_LIBS	= -lpam -ldl

LIBPROGS        = $(PAM_AUTH_EXE)
OBJS	 	= pam_auth.o

all:    $(PAM_AUTH_EXE)

$(PAM_AUTH_EXE): pam_auth.o
	$(CC) $(LDFLAGS) pam_auth.o -o $@ $(AUTH_LIBS)

install-mkdirs:
	-@if test ! -d $(prefix); then \
		echo "mkdir $(prefix)"; \
		mkdir $(prefix); \
	fi
	-@if test ! -d $(libexecdir); then \
		echo "mkdir $(libexecdir)"; \
		mkdir $(libexecdir); \
	fi

# Michael Lupp <mike@nemesis.saar.de> wants to know about additions
# to the install target.
install: all install-mkdirs
	@for f in $(LIBPROGS); do \
		if test -f $(libexecdir)/$$f; then \
			echo $(MV) $(libexecdir)/$$f $(libexecdir)/-$$f; \
			$(MV) $(libexecdir)/$$f $(libexecdir)/-$$f; \
		fi; \
		echo $(INSTALL_BIN) $$f $(libexecdir); \
		$(INSTALL_BIN) $$f $(libexecdir); \
		if test -f $(libexecdir)/-$$f; then \
			echo $(RM) -f $(libexecdir)/-$$f; \
			$(RM) -f $(libexecdir)/-$$f; \
		fi; \
	done

clean: 
	-rm -rf *.o *pure_* core $(LIBPROGS)

distclean:	clean
	-rm -f Makefile

tags:
	ctags *.[ch]

depend:
	$(MAKEDEPEND) -fMakefile *.c
