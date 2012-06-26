.SUFFIXES : .x .o .c .s

BUILD_ROOT=/usr/local/arm_tools/
lib=${BUILD_ROOT}arm-elf/lib
lib1=${BUILD_ROOT}lib/gcc-lib/arm-elf/3.0/
include=${BUILD_ROOT}arm-elf/inc

CC=arm-elf-gcc -I${include} -DNEW_KERNEL -DSTANDARD -O1 -c
AS=arm-elf-as
LD=arm-elf-ld

WEC_LDFLAGS=-L./ -L${lib} -L${lib}/lib -L${lib}/libc -L${lib1} -T${lib}/elf2flt.ld

EXEC1 = pptp
OBJS1 = pptp.o pptp_gre.o ppp_fcs.o \
            pptp_ctrl.o dirutil.o vector.o \
            inststr.o util.o version.o \
	    pptp_quirks.o orckit_quirks.o pqueue.o utils.o

### DEPS
### pptp_callmgr.h pptp_gre.h ppp_fcs.h pty.h util.h

EXEC2 = pptp_callmgr
OBJS2 = pptp_callmgr.o pptp_gre.o ppp_fcs.o \
            pptp_ctrl.o dirutil.o vector.o \
            inststr.o util.o version.o \
	    pptp_quirks.o orckit_quirks.o pqueue.o utils.o

LIBS=-lc  -lgcc -lc

all:config.h $(EXEC1).x $(EXEC2).x

	elf2flt -o $(EXEC1) $(EXEC1).x  >temp 2>&1
	flthdr -z  $(EXEC1)
	cp $(EXEC1) ../../bin

	elf2flt -o $(EXEC2) $(EXEC2).x  >temp 2>&1
	flthdr -z  $(EXEC2)
	cp $(EXEC2) ../../bin


$(EXEC1).x: $(OBJS1)
	$(LD) -o $(EXEC1).x $(WEC_LDFLAGS) $(OBJS1) $(LIBS)
	rm -f $(EXEC1).x
	$(LD) -o $(EXEC1).x $(WEC_LDFLAGS) -r -d  $(OBJS1) $(LIBS) -Map $(EXEC1).map
$(EXEC2).x: $(OBJS2)
	$(LD) -o $(EXEC2).x $(WEC_LDFLAGS) $(OBJS2) $(LIBS)
	rm -f $(EXEC2).x
	$(LD) -o $(EXEC2).x $(WEC_LDFLAGS) -r -d  $(OBJS2) $(LIBS) -Map $(EXEC2).map

config.h: 
	echo "/* text added by Makefile target config.h */" > config.h
	echo "#define PPTP_LINUX_VERSION \"$(VERSION)$(RELEASE)\"" >> config.h
	echo "#define PPPD_BINARY \"$(PPPD)\"" >> config.h

.c.o:
	$(CC) $*.c

.s.o:
	$(AS) -ahld -o $*.o $*.s > NULL
	del NULL

	 
clean:
	rm -f *.o 
	rm -f *.x 
	rm -f *.flat
	rm -f *.bak
	rm -f $(TARGET)
	rm -f *.map
	rm -f temp
	rm -f $(EXEC1)
	rm -f $(EXEC2)
