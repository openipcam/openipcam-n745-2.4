#
# linux/drivers/maps/Makefile.24
# Makefile for obsolete kernels
#
# $Id: Makefile,v 1.1.1.1 2006-07-11 09:29:06 andy Exp $

O_TARGET	:= devlink.o
export-objs	:= docecc.o

obj-$(CONFIG_MTD_DOC2000)       += doc2000.o
obj-$(CONFIG_MTD_DOC2001)       += doc2001.o
obj-$(CONFIG_MTD_DOC2001PLUS)   += doc2001plus.o
obj-$(CONFIG_MTD_DOCPROBE)      += docprobe.o
obj-$(CONFIG_MTD_DOCECC)        += docecc.o
obj-$(CONFIG_MTD_SLRAM)         += slram.o
obj-$(CONFIG_MTD_PHRAM)         += phram.o
obj-$(CONFIG_MTD_PMC551)        += pmc551.o
obj-$(CONFIG_MTD_MS02NV)        += ms02-nv.o
obj-$(CONFIG_MTD_MTDRAM)        += mtdram.o
obj-$(CONFIG_MTD_LART)          += lart.o
obj-$(CONFIG_MTD_BLKMTD)        += blkmtd-24.o

include $(TOPDIR)/Rules.make
