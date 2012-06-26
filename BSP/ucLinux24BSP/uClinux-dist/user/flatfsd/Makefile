######################################################

EXEC = flatfsd
OBJS = flatfsd.o newfs.o reboot.o
# FLTFLAGS += -s 2048

# force error checking on compile
CFLAGS += -Wall -Werror

######################################################
#
# Erk! settings to change the way flatfsd works
#
USING_FLASH_FILESYSTEM_$(CONFIG_USER_FLATFSD_USE_FLASH_FS) = y
ifdef CONFIG_NFTL
USING_DISKLIKE_DEVICE_$(CONFIG_SH_SECUREEDGE5410) = y
endif
ifeq ($(USING_DISKLIKE_DEVICE_y),)
  USING_MTD_DEVICE_$(CONFIG_MTD) = y
  USING_BLKMEM_DEVICE_$(CONFIG_BLK_DEV_BLKMEM) = y
endif

######################################################

ifdef USING_DISKLIKE_DEVICE_y
CFLAGS += -DUSING_DISKLIKE_DEVICE=1
endif

ifdef USING_MTD_DEVICE_y
CFLAGS += -DUSING_MTD_DEVICE=1
endif

ifdef USING_BLKMEM_DEVICE_y
CFLAGS += -DUSING_BLKMEM_DEVICE=1
endif

ifdef USING_FLASH_FILESYSTEM_y
CFLAGS += -DUSING_FLASH_FILESYSTEM=1
else
OBJS += flatfs.o 
endif


######################################################

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

######################################################
