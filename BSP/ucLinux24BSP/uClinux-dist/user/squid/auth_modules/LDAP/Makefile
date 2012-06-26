# Generated automatically from Makefile.in by configure.

OBJS		= squid_ldap_auth.o
LIBS		= -lldap -llber
LDAP_EXE	= squid_ldap_auth

prefix		= /usr/squid
exec_prefix	= ${prefix}
exec_suffix	= 
cgi_suffix	= .cgi
top_srcdir	= ../..
bindir		= ${exec_prefix}/bin
libexecdir	= ${bindir}
sysconfdir	= ${prefix}/etc
localstatedir	= ${prefix}
srcdir		= .

CC		= gcc
MAKEDEPEND	= /usr/bin/makedepend
INSTALL		= /usr/bin/install -c
INSTALL_BIN	= ${INSTALL}
INSTALL_FILE	= ${INSTALL} -m 644
INSTALL_SUID	= ${INSTALL} -o root -m 4755
RANLIB		= ranlib
LN_S		= ln -s
PERL		= /usr/bin/perl
CRYPTLIB	= -lcrypt
REGEXLIB	= -lregex
PTHREADLIB	= 
MALLOCLIB	= 
AC_CFLAGS	= -g -O2 -Wall
LDFLAGS		= -g
XTRA_LIBS	= 
XTRA_OBJS	= 
MV		= /bin/mv
RM		= /bin/rm
SHELL		= /bin/sh


all: $(LDAP_EXE)

$(LDAP_EXE): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

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
	@for f in $(LDAP_EXE); do \
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
	-$(RM) -f $(OBJS)
	-$(RM) -f $(LDAP_EXE)

distclean: clean
	-$(RM) -f Makefile
