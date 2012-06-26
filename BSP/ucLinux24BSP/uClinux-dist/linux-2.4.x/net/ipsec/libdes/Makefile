
.EXPORT_ALL_VARIABLES:

EXTRA_CFLAGS += -I$(ROOTDIR)/freeswan/libdes -I$(ROOTDIR)/lib/$(LIBCDIR)/include

ifdef CONFIG_MODULES
EXTRA_CFLAGS += -DEXPORT_SYMTAB
endif

VPATH = $(ROOTDIR)/freeswan/libdes

ifdef CONFIG_COLDFIRE
COLDFIRE_KERNEL = y # get the ASM functions
endif


include $(ROOTDIR)/freeswan/libdes/Makefile

