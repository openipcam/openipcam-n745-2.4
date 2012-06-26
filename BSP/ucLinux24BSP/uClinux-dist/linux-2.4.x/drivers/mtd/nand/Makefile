#
# linux/drivers/nand/Makefile.24
# Makefile for obsolete kernels.
#
# $Id: Makefile,v 1.1.1.1 2006-07-11 09:29:06 andy Exp $

O_TARGET	:= nandlink.o
export-objs	:= nand_base.o nand_bbt.o nand_ecc.o nand_ids.o
list-multi	:= nand.o

include Makefile.common

include $(TOPDIR)/Rules.make

nand.o: $(nand-objs)
	$(LD) -r -o $@ $(nand-objs)
