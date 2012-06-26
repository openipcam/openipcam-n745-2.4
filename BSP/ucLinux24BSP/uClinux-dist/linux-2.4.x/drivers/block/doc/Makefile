#
# Makefile for a block device driver
# for the DiskOnChip flash module from M-Systems
#
# Copyright (C) 1998 IGEL GmbH
#
# 16.8.98 updated by Ron Dick, rond@m-sys.com
#
# 15 January 2001, Tom Rushworth <tbr@lineo.com>
# Rewritten to use lists instead of if-statements.
#

O_TARGET := doc.o

CFLAGS		+= -DCOTULLA

list-multi      := doc.o

LIBOSAKDIR =./lib
OSAKDIR = ./Tffs511/src/

obj-$(CONFIG_BLK_DEV_MSYS_DOC)	+= doc.o

doc-objs   := fldrvlnx.o tsi_doc.o $(LIBOSAKDIR)libosak.a



OBJS := $(LIBOSAKDIR)blockdev.o $(LIBOSAKDIR)flsocket.o $(LIBOSAKDIR)flflash.o \
	$(LIBOSAKDIR)fltl.o $(LIBOSAKDIR)flioctl.o $(LIBOSAKDIR)fatfilt.o \
	$(LIBOSAKDIR)docsys.o $(LIBOSAKDIR)reedsol.o $(LIBOSAKDIR)diskonc.o \
	$(LIBOSAKDIR)nftllite.o $(LIBOSAKDIR)flbase.o $(LIBOSAKDIR)dosformt.o \
	$(LIBOSAKDIR)docbdk.o $(LIBOSAKDIR)inftl.o $(LIBOSAKDIR)mdocplus.o \
	$(LIBOSAKDIR)protectp.o $(LIBOSAKDIR)fllnx.o $(LIBOSAKDIR)docsoc.o \
	$(LIBOSAKDIR)flcustom.o  $(LIBOSAKDIR)flsystem.o




$(LIBOSAKDIR)blockdev.o: $(OSAKDIR)blockdev.c
	touch $@
$(LIBOSAKDIR)doc2exb.o: $(OSAKDIR)doc2exb.c
	touch $@
$(LIBOSAKDIR)docbdk.o: $(OSAKDIR)docbdk.c
	touch $@
$(LIBOSAKDIR)docsoc.o: $(OSAKDIR)docsoc.c
	touch $@
$(LIBOSAKDIR)diskonc.o: $(OSAKDIR)diskonc.c
	touch $@
$(LIBOSAKDIR)dosformt.o: $(OSAKDIR)dosformt.c
	touch $@
$(LIBOSAKDIR)fatfilt.o: $(OSAKDIR)fatfilt.c
	touch $@
$(LIBOSAKDIR)flbase.o: $(OSAKDIR)flbase.c
	touch $@
$(LIBOSAKDIR)flflash.o: $(OSAKDIR)flflash.c
	touch $@
$(LIBOSAKDIR)flioctl.o: $(OSAKDIR)flioctl.c
	touch $@
$(LIBOSAKDIR)flsocket.o: $(OSAKDIR)flsocket.c
	touch $@
$(LIBOSAKDIR)fltl.o: $(OSAKDIR)fltl.c
	touch $@
$(LIBOSAKDIR)inftl.o: $(OSAKDIR)inftl.c
	touch $@
$(LIBOSAKDIR)mdocplus.o: $(OSAKDIR)mdocplus.c
	touch $@
$(LIBOSAKDIR)mtdsa.o: $(OSAKDIR)mtdsa.c
	touch $@
$(LIBOSAKDIR)nftllite.o: $(OSAKDIR)nftllite.c
	touch $@
$(LIBOSAKDIR)protectp.o: $(OSAKDIR)protectp.c
	touch $@
$(LIBOSAKDIR)reedsol.o: $(OSAKDIR)reedsol.c
	touch $@
$(LIBOSAKDIR)fllnx.o: $(LIBOSAKDIR)fllnx.c
	touch $@
$(LIBOSAKDIR)flcustom.o: $(LIBOSAKDIR)flcustom.c
	touch $@
$(LIBOSAKDIR)flsystem.o: $(LIBOSAKDIR)flsystem.c
	touch $@
$(LIBOSAKDIR)docsys.o: $(LIBOSAKDIR)docsys.c
	touch $@
		
include $(TOPDIR)/Rules.make

doc.o: $(doc-objs)
	$(LD) -r -o $@ $(doc-objs)

$(LIBOSAKDIR)libosak.a: $(OBJS)
	$(AR) ru $@ $?
	

# Yes, this looks weird, and yes this does seem to be needed sometimes
# to make sure changes to compat* get noticed for fldrvlnx.o
fldrvlnx.c: compat24.h compat24.c
	touch $@

tsi_doc.c: compat24.h compat24.c
	touch $Q

