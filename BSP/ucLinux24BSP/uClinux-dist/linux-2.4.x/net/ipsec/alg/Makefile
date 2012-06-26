
.EXPORT_ALL_VARIABLES:

EXTRA_CFLAGS += -I$(ROOTDIR)/freeswan -I$(ROOTDIR)/freeswan/lib \
			-I$(ROOTDIR)/freeswan/klips/net/ipsec/alg \
			-I$(ROOTDIR)/freeswan/libdes

vpath %.h $(ROOTDIR)/freeswan/klips/net/ipsec/alg
vpath %.c $(ROOTDIR)/freeswan/klips/net/ipsec/alg

# special hack required because of exported object files
# %.c: $(ROOTDIR)/freeswan/klips/net/ipsec/alg/%.c
#	ln -fs $< $@

include $(ROOTDIR)/freeswan/klips/net/ipsec/alg/Makefile

