# Generated automatically from Makefile.in by configure.
# $Id: Makefile,v 1.1.1.1 2006-07-11 09:33:05 andy Exp $
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
DEFAULT_ICON_DIR        = $(ROMFSDIR)$(prefix)/icons

ICONS		= anthony-binhex.gif \
		  anthony-bomb.gif \
		  anthony-box.gif \
		  anthony-box2.gif \
		  anthony-c.gif \
		  anthony-compressed.gif \
		  anthony-dir.gif \
		  anthony-dirup.gif \
		  anthony-dvi.gif \
		  anthony-f.gif \
		  anthony-image.gif \
		  anthony-image2.gif \
		  anthony-layout.gif \
		  anthony-link.gif \
		  anthony-movie.gif \
		  anthony-pdf.gif \
		  anthony-portal.gif \
		  anthony-ps.gif \
		  anthony-quill.gif \
		  anthony-script.gif \
		  anthony-sound.gif \
		  anthony-tar.gif \
		  anthony-tex.gif \
		  anthony-text.gif \
		  anthony-unknown.gif \
		  anthony-xbm.gif \
		  anthony-xpm.gif

all:

icons.shar:
	shar --no-timestamp $(ICONS) > $@

install: install-mkdirs anthony-xpm.gif
	@for f in $(ICONS); do \
		if test -f $(DEFAULT_ICON_DIR)/$$f ; then \
			:; \
		else \
			echo "$(INSTALL_FILE) $$f $(DEFAULT_ICON_DIR)"; \
			$(INSTALL_FILE) $$f $(DEFAULT_ICON_DIR); \
		fi; \
	done

install-mkdirs:
	-@if test ! -d $(ROMFSDIR)/$(prefix); then \
		echo "mkdir $(prefix)"; \
		mkdir $(ROMFSDIR)/$(prefix); \
	fi
	-@if test ! -d $(DEFAULT_ICON_DIR); then \
		echo "mkdir $(DEFAULT_ICON_DIR)"; \
		mkdir $(DEFAULT_ICON_DIR); \
	fi

clean:


distclean: clean
	-rm -f Makefile
