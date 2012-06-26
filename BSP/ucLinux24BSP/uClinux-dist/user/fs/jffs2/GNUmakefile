# $Id: GNUmakefile,v 1.1.1.1 2006-07-11 09:31:07 andy Exp $

LINUXDIR=/lib/modules/$(shell uname -r)/build

ifndef VERSION

# Someone just typed 'make'. Invoke the kernel makefiles properly...
OUT_OF_TREE_BUILD=1
export OUT_OF_TREE_BUILD

modules:
	make -C $(LINUXDIR) SUBDIRS=`pwd` mtd=`pwd` modules

nonand:
	make -C $(LINUXDIR) SUBDIRS=`pwd` mtd=`pwd` NONAND=1 modules

dep:
	make -C $(LINUXDIR) SUBDIRS=`pwd` mtd=`pwd` dep

clean:
	rm -f `find . -name \*.ko` `find . -name \*.o`

veryclean:
	for a in `find . -type f  -a \! -path \*/CVS/\*` ; do  \
		grep -q ^/`basename $$a`/ `dirname $$a`/CVS/Entries ||  \
			rm -v $$a ; \
	done

else

# 2.5 explicitly states <subdir>/Makefile rather than using recursive make,
# so we handle all that in there...

obj := $(shell pwd)
include Makefile

endif
