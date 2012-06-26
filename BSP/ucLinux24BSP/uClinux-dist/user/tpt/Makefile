# $Header: /usr/local/cvsroot/W90N745/uClinux-dist/user/tpt/Makefile,v 1.1.1.1 2006-07-11 09:33:41 andy Exp $

CFLAGS+=-Wall -Wshadow

OFILES = tpt.o util.o build.o dump.o massage.o

tpt: $(OFILES)

romfs:
	$(ROMFSINST) /bin/tpt

$(OFILES): tpt.h

clean:
	rm -f $(OFILES) tpt
