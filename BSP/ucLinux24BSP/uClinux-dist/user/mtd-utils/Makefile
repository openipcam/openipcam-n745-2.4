
# -*- sh -*-

# $Id: Makefile,v 1.1.1.1 2006-07-11 09:31:28 andy Exp $

SBINDIR=/usr/sbin
MANDIR=/usr/man
INCLUDEDIR=/usr/include
#CROSS=arm-linux-
CC := $(CROSS)gcc
_CFLAGS := -I../include -O2 -Wall
_CFLAGS+=$(CFLAGS)
CFLAGS:= $(_CFLAGS)

TARGETS = ftl_format flash_erase flash_eraseall nanddump doc_loadbios \
	mkfs.jffs ftl_check mkfs.jffs2 flash_lock flash_unlock \
	flash_info mtd_debug flashcp nandwrite jffs2dump \
	nftldump nftl_format docfdisk #jffs2reader

SYMLINKS = compr_lzari.c compr_lzo.c

%: %.o
	$(CC) $(LDFLAGS) -g -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -g -c -o $@ $< -g -Wp,-MD,.$<.dep

.SUFFIXES:

all: $(TARGETS)

IGNORE=${wildcard .*.c.dep} 
-include ${IGNORE}

clean:
	rm -f *.o *.gdb $(TARGETS) .*.c.dep $(SYMLINKS)

$(SYMLINKS):
	ln -sf ../fs/jffs2/$@ $@

mkfs.jffs2: crc32.o compr_rtime.o mkfs.jffs2.o compr_zlib.o compr.o compr_lzari.o compr_lzo.o
	$(CC) $(LDFLAGS) -o $@ $^ -lz

flash_eraseall: crc32.o flash_eraseall.o
	$(CC) $(LDFLAGS) -o $@ $^

jffs2reader: jffs2reader.o
	$(CC) $(LDFLAGS) -o $@ $^ -lz 

jffs2dump: jffs2dump.o crc32.o
	$(CC) $(LDFLAGS) -o $@ $^

install: ${TARGETS}
	mkdir -p ${DESTDIR}/${SBINDIR}
	install -m0755 -oroot -groot ${TARGETS} ${DESTDIR}/${SBINDIR}/
	mkdir -p ${DESTDIR}/${MANDIR}/man1
	gzip -c mkfs.jffs2.1 > ${DESTDIR}/${MANDIR}/man1/mkfs.jffs2.1.gz
	mkdir -p ${DESTDIR}/${INCLUDEDIR}/mtd
	install -m0644 -oroot -groot ../include/mtd/*.h ${DESTDIR}/${INCLUDEDIR}/mtd/
