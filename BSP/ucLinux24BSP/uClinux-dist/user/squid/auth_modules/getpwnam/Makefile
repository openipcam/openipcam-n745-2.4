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
top_srcdir	= ../..
bindir		= ${exec_prefix}/bin
srcdir		= .

# Gotta love the DOS legacy
#
GETPWNAM_AUTH_EXE	= getpwnam_auth$(exec_suffix)

CC		= gcc
MAKEDEPEND	= /usr/bin/makedepend
INSTALL		= /usr/bin/install -c
INSTALL_BIN	= ${INSTALL}
CRYPTLIB	= -lcrypt
AC_CFLAGS	= -g -O2 -Wall
LDFLAGS		= -g
XTRA_LIBS	= 
XTRA_OBJS	= 
MV		= /bin/mv
RM		= /bin/rm
SHELL		= /bin/sh


INCLUDE		= -I. -I../../include -I$(top_srcdir)/include
CFLAGS		= $(AC_CFLAGS) $(INCLUDE) $(DEFINES)
AUTH_LIBS	= -L../../lib -lmiscutil $(CRYPTLIB) $(XTRA_LIBS)

PROGS		= $(GETPWNAM_AUTH_EXE)
OBJS		= getpwnam_auth.o

all:    $(GETPWNAM_AUTH_EXE)

$(OBJS): $(top_srcdir)/include/version.h

$(GETPWNAM_AUTH_EXE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(AUTH_LIBS)

install-mkdirs:
	-@if test ! -d $(prefix); then \
		echo "mkdir $(prefix)"; \
		mkdir $(prefix); \
	fi
	-@if test ! -d $(bindir); then \
		echo "mkdir $(bindir)"; \
		mkdir $(bindir); \
	fi

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

distclean:     clean
	-rm -f Makefile

depend:
	$(MAKEDEPEND) -I../include -I. -fMakefile *.c
