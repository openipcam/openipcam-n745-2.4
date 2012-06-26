# Generated automatically from Makefile.in by configure.
# $Id: Makefile,v 1.1.1.1 2006-07-11 09:33:00 andy Exp $
#
prefix		= /usr/squid
exec_prefix	= ${prefix}
top_srcdir	= ..
bindir		= ${exec_prefix}/bin
libexecdir      = ${bindir}
sysconfdir	= ${prefix}/etc
localstatedir   = ${prefix}
srcdir		= .
SH		= /bin/sh
RM		= /bin/rm
SHELL		= /bin/sh

INSTALL         	= /usr/bin/install -c
INSTALL_FILE    	= ${INSTALL} -m 644
DEFAULT_ERROR_DIR	= $(ROMFSDIR)$(prefix)/errors

LANGUAGE		= English

all:

install: install-mkdirs
	@for f in $(srcdir)/$(LANGUAGE)/ERR_*; do \
		if test -f $(DEFAULT_ERROR_DIR)/$$f ; then \
			:; \
		else \
			echo "$(INSTALL_FILE) $$f $(DEFAULT_ERROR_DIR)"; \
			$(INSTALL_FILE) $$f $(DEFAULT_ERROR_DIR); \
		fi; \
	done

install-mkdirs:
	-@if test ! -d $(ROMFSDIR)/$(prefix); then \
		echo "mkdir $(prefix)"; \
		mkdir $(ROMFSDIR)/$(prefix); \
	fi
	-@if test ! -d $(DEFAULT_ERROR_DIR); then \
		echo "mkdir $(DEFAULT_ERROR_DIR)"; \
		mkdir $(DEFAULT_ERROR_DIR); \
	fi

# undocumented hack.  You can use this target to create multi-lingual
# error pages.  For example:
#
#	make ADDLANG=English addlang
#
# by Andres Kroonmaa <andre@mail.lbi.ee>
#
addlang: all
	-@if test -d $(srcdir)/$(ADDLANG); then \
	cd $(srcdir)/$(ADDLANG)/ ; \
	for f in ERR_*; do \
		if test -f $(DEFAULT_ERROR_DIR)/$$f ; then \
			echo "appending $(ADDLANG)/$$f"; \
			echo "<HR>" >> $(DEFAULT_ERROR_DIR)/$$f ; \
			cat $$f >> $(DEFAULT_ERROR_DIR)/$$f; \
		fi; \
	done; \
	fi

clean:

distclean: clean
	-rm -f Makefile
