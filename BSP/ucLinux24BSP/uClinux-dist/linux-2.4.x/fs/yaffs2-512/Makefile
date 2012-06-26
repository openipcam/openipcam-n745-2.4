#
# fs/jffs2/Makefile.24
#
# $Id: Makefile,v 1.1.1.1 2007-07-31 05:18:10 andy Exp $

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


obj-$(CONFIG_YAFFS_FS) += yaffs2-512.o


YAFFS2_OBJS := yaffs_ecc.o yaffs_fs.o yaffs_guts.o
YAFFS2_OBJS += yaffs_packedtags2.o
YAFFS2_OBJS += yaffs_tagscompat.o yaffs_tagsvalidity.o yaffs_mtdif.o yaffs_mtdif2.o yaffs_nand.o yaffs_checkptrw.o


obj-y := $(YAFFS2_OBJS)
O_TARGET := yaffs2-512.o

include $(TOPDIR)/Rules.make

