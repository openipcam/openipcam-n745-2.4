#
# Out of tree fs/jffs2/Makefile
#
# $Id: Makefile,v 1.1.1.1 2006-07-11 09:31:07 andy Exp $

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
include $(obj)/Makefile.common
