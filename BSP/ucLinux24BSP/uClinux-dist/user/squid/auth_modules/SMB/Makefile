# Generated automatically from Makefile.in by configure.
#
#  Makefile for the Squid Object Cache server
#
#  $Id: Makefile,v 1.1.1.1 2006-07-11 09:32:59 andy Exp $
#
#  Uncomment and customize the following to suit your needs:
#

# SAMBAPREFIX must point to the directory where Samba has been installed.
# By default, Samba is installed in /usr/local/samba. If you changed this
# by using the --prefix option when configuring Samba, you need to change
# SAMBAPREFIX accordingly.

SAMBAPREFIX=/usr/local/samba

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
SMB_AUTH_EXE	= smb_auth$(exec_suffix)
SMB_AUTH_HELPER	= smb_auth.sh
SMB_AUTH_HELPER_PATH = $(libexecdir)/$(SMB_AUTH_HELPER)

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
DEFINES		= -DSAMBAPREFIX=\"$(SAMBAPREFIX)\" -DHELPERSCRIPT=\"$(SMB_AUTH_HELPER_PATH)\"

INCLUDE		= -I. -I../../include -I$(top_srcdir)/include
CFLAGS 		= $(AC_CFLAGS) $(INCLUDE) $(DEFINES)
AUTH_LIBS	= 

LIBPROGS        = $(SMB_AUTH_EXE) $(SMB_AUTH_HELPER)
OBJS	 	= smb_auth.o

all:    $(SMB_AUTH_EXE)

$(SMB_AUTH_EXE): smb_auth.o
	$(CC) $(LDFLAGS) smb_auth.o -o $@ $(AUTH_LIBS)

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
	-rm -rf *.o *pure_* core $(PROGS)

distclean:	clean
	-rm -f Makefile

tags:
	ctags *.[ch]

depend:
	$(MAKEDEPEND) -fMakefile *.c
