#
# fs/jffs2/Makefile.24
#
# $Id: Makefile,v 1.1.1.1 2006-07-11 09:29:47 andy Exp $

ifdef OUT_OF_TREE_BUILD
include $(mtd)/defconfig

# This must be first in the include path, so it goes in $(CC) rather
# then $(EXTRA_CFLAGS)

CC += -I$(mtd)/../../include
EXTRA_CFLAGS := -g -Werror

ifndef CONFIG_MTD
EXTRA_CFLAGS += -DMTD_OUT_OF_TREE
endif

ifdef NONAND
EXTRA_CFLAGS += -DNONAND
endif
endif

obj-$(CONFIG_JFFS2_FS) += jffs2.o

JFFS2_OBJS := compr.o dir.o file.o ioctl.o nodelist.o malloc.o
JFFS2_OBJS += read.o nodemgmt.o readinode.o write.o scan.o gc.o
JFFS2_OBJS += symlink-v24.o build.o erase.o background.o fs.o writev.o

LINUX_OBJS += super-v24.o crc32.o rbtree.o

NAND_OBJS-$(CONFIG_JFFS2_FS_NAND) += wbuf.o

COMPR_OBJS-$(CONFIG_JFFS2_RUBIN)  += compr_rubin.o
COMPR_OBJS-$(CONFIG_JFFS2_RTIME)  += compr_rtime.o
COMPR_OBJS-$(CONFIG_JFFS2_ZLIB)   += compr_zlib.o

obj-y := $(COMPR_OBJS-y) $(JFFS2_OBJS) $(LINUX_OBJS) $(NAND_OBJS-y)
O_TARGET := jffs2.o

include $(TOPDIR)/Rules.make

