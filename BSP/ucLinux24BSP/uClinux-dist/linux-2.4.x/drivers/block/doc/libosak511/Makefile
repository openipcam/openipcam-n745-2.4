#mkefile for libosak.uu, part of linux driver
OSAKDIR = ../Tffs511/src/
LIBOSAKDIR =./
DRIVERDIR = ../
#INCLUDEDIR = /usr/local/arm-linux/linux/include/

OBJS = $(LIBOSAKDIR)blockdev.o $(LIBOSAKDIR)flsocket.o $(LIBOSAKDIR)flflash.o \
	$(LIBOSAKDIR)fltl.o $(LIBOSAKDIR)flioctl.o $(LIBOSAKDIR)fatfilt.o \
	$(LIBOSAKDIR)docsys.o $(LIBOSAKDIR)reedsol.o $(LIBOSAKDIR)diskonc.o \
	$(LIBOSAKDIR)nftllite.o $(LIBOSAKDIR)flbase.o $(LIBOSAKDIR)dosformt.o \
	$(LIBOSAKDIR)docbdk.o $(LIBOSAKDIR)inftl.o $(LIBOSAKDIR)mdocplus.o \
	$(LIBOSAKDIR)protectp.o $(LIBOSAKDIR)fllnx.o $(LIBOSAKDIR)docsoc.o \
	$(LIBOSAKDIR)flcustom.o  $(LIBOSAKDIR)flsystem.o

# Uncomment this if you are using Redhat 7
# CC=kgcc
CC = arm-elf-gcc
AR = arm-elf-ar

# CFLAGS = -I$(OSAKDIR) -I$(LIBOSAKDIR) -DMODULE -D__CMP__ -D__KERNEL__  -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer -fno-strength-reduce -DDEBUG -c
CFLAGS = -I$(OSAKDIR) -I$(LIBOSAKDIR) -I$(INCLUDEDIR) -DMODULE -D__KERNEL__  \
	-DEXTERNAL_MUTEX -DFLITE -Wall -Wstrict-prototypes -O2               \
	-fomit-frame-pointer -fno-strength-reduce -march=armv4 -Wa,-mxscale  \
	-mtune=strongarm -mshort-load-bytes -c

#CFLAGS := $(CPPFLAGS) -Wall -Wstrict-prototypes -Wno-trigraphs -O2 \
#	  -fomit-frame-pointer -fno-strict-aliasing -fno-common


all: $(LIBOSAKDIR)libosak.a $(LIBOSAKDIR)libosak.uu

$(LIBOSAKDIR)libosak.uu: $(LIBOSAKDIR)libosak.a
	uuencode libosak.a libosak.a > libosak.uu
	cp $(LIBOSAKDIR)fllnx.h $(LIBOSAKDIR)libosak.uu $(DRIVERDIR)

$(LIBOSAKDIR)libosak.a: $(OBJS)
	$(AR) ru $@ $?

$(LIBOSAKDIR)blockdev.o: $(OSAKDIR)blockdev.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)doc2exb.o: $(OSAKDIR)doc2exb.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)docbdk.o: $(OSAKDIR)docbdk.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)docsoc.o: $(OSAKDIR)docsoc.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)diskonc.o: $(OSAKDIR)diskonc.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)dosformt.o: $(OSAKDIR)dosformt.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)fatfilt.o: $(OSAKDIR)fatfilt.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)flbase.o: $(OSAKDIR)flbase.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)flflash.o: $(OSAKDIR)flflash.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)flioctl.o: $(OSAKDIR)flioctl.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)flsocket.o: $(OSAKDIR)flsocket.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)fltl.o: $(OSAKDIR)fltl.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)inftl.o: $(OSAKDIR)inftl.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)mdocplus.o: $(OSAKDIR)mdocplus.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)mtdsa.o: $(OSAKDIR)mtdsa.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)nftllite.o: $(OSAKDIR)nftllite.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)protectp.o: $(OSAKDIR)protectp.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)reedsol.o: $(OSAKDIR)reedsol.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)fllnx.o: $(LIBOSAKDIR)fllnx.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)flcustom.o: $(LIBOSAKDIR)flcustom.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)flsystem.o: $(LIBOSAKDIR)flsystem.c
	$(CC) $(CFLAGS) $? -o $@
$(LIBOSAKDIR)docsys.o: $(LIBOSAKDIR)docsys.c
	$(CC) $(CFLAGS) $? -o $@

clean:
	-rm -v $(LIBOSAKDIR)*.o $(LIBOSAKDIR)*.a $(LIBOSAKDIR)*.uu

install: libosak.uu
	cp $(LIBOSAKDIR)fllnx.h $(LIBOSAKDIR)libosak.uu $(DRIVERDIR)

