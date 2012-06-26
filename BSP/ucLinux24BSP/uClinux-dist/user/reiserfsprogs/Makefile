
.EXPORT_ALL_VARIABLES:

CFLAGS +=  -DNO_STRFTIME #mcli del -Dlseek64=reiserfs_llseek

DIRS = lib reiserfscore mkreiserfs fsck debugreiserfs resize_reiserfs

all:
	for i in $(DIRS) ; do make -C $$i || exit 1 ; done

romfs:
	for i in $(DIRS) ; do make -C $$i romfs || exit 1 ; done

clean:
	-for i in $(DIRS) ; do make -C $$i clean ; done


