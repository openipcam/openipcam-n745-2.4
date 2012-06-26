# Generated automatically from Makefile.in by configure.
#
#  $Id: Makefile,v 1.1.1.1 2006-07-11 09:33:06 andy Exp $
#
prefix		= /usr/squid
top_srcdir	= ..
srcdir		= .


# CC		= gcc
BUILD_CC	= gcc
MAKEDEPEND	= /usr/bin/makedepend
INSTALL		= /usr/bin/install -c
INSTALL_BIN 	= ${INSTALL}
INSTALL_FILE 	= ${INSTALL} -m 644
# RANLIB		= ranlib
AC_CFLAGS	= 
#LDFLAGS		= -g
XTRA_LIBS	= 
RM		= /bin/rm
LIBOBJS		=  drand48.o tempnam.o
AR_R		= $(AR) r

INCLUDE		= -I../include -I$(top_srcdir)/include
UTILOBJS	= rfc1123.o \
		  rfc1738.o \
		  rfc1035.o \
		  util.o \
		  getfullhostname.o \
		  base64.o \
		  uudecode.o \
		  splay.o \
		  safe_inet_addr.o \
		  iso3307.o \
		  snprintf.o \
		  md5.o \
		  radix.o \
		  stub_memaccount.o \
		  Array.o \
		  Stack.o \
		  hash.o \
		  heap.o \
		  $(LIBOBJS)
REGEXOBJS	= GNUregex.o
DLMALLOCOBJS	= dlmalloc.o
LIBS		= libmiscutil.a libregex.a libhostutil.a

CFLAGS		+= $(AC_CFLAGS) $(INCLUDE)
BUILD_CFLAGS	= $(AC_FLAGS) $(INCLUDE)

all: $(LIBS)

.c.o:
	$(BUILD_CC) -c $(BUILD_CFLAGS) $(CPPFLAGS) -o $@ $<
	mv $@ host-$@
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

$(UTILOBJS): $(top_srcdir)/include/version.h

libhostutil.a: $(UTILOBJS)
	$(RM) -f $@
	for i in $(UTILOBJS); do ar -r $@ host-$$i; done
	ranlib $@

libmiscutil.a: $(UTILOBJS)
	$(RM) -f $@
	$(AR_R) $@ $(UTILOBJS)
	$(RANLIB) $@

libregex.a: $(REGEXOBJS)
	$(RM) -f $@
	$(AR_R) $@ $(REGEXOBJS)
	$(RANLIB) $@

libdlmalloc.a: $(DLMALLOCOBJS)
	$(RM) -f $@
	$(AR_R) $@ $(DLMALLOCOBJS)
	$(RANLIB) $@

clean:
	-rm -f *.o $(LIBS) core

distclean: clean
	-rm -f libregex.a
	-rm -f Makefile Makefile.bak

install: all

depend:
	$(MAKEDEPEND) -I../include -I. -fMakefile *.c

