ifeq ($(CONFIG_IPSEC),)
fastdep:
else
.EXPORT_ALL_VARIABLES:

EXTRA_CFLAGS += -I$(ROOTDIR)/freeswan -I$(ROOTDIR)/freeswan/lib \
			-I$(ROOTDIR)/freeswan/klips/net/ipsec \
			-I$(ROOTDIR)/freeswan/libdes

vpath %.h $(ROOTDIR)/freeswan/klips/net/ipsec

# special hack required because of exported object files
%.c: $(ROOTDIR)/freeswan/klips/net/ipsec/%.c
	ln -fs $< $@

include $(ROOTDIR)/freeswan/klips/net/ipsec/Makefile
endif
