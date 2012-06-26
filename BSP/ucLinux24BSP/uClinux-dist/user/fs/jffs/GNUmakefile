# GNUmakefile for JFFS build outside kernel tree
# $Id: GNUmakefile,v 1.1.1.1 2006-07-11 09:31:07 andy Exp $

LINUXDIR=/usr/src/linux

ifndef VERSION

# Someone just typed 'make'

modules:
	make -C $(LINUXDIR) SUBDIRS=`pwd` modules

dep:
	make -C $(LINUXDIR) SUBDIRS=`pwd` dep

clean:
	rm -f *.o

else

ifndef CONFIG_JFFS_FS

CC += -I$(shell pwd)/../../include

CONFIG_JFFS_FS := m
CONFIG_JFFS_PROC_FS := y
EXTRA_CFLAGS += -DCONFIG_JFFS_FS_VERBOSE=1 -g -DCONFIG_JFFS_PROC_FS=1

endif

include Makefile


endif
