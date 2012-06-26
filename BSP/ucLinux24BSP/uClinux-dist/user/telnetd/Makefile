
EXEC = telnetd
OBJS = telnetd.o state.o termstat.o slc.o sys_term.o \
	utility.o global.o authenc.o logwtmp.o logout.o

CFLAGS += -DPARANOID_TTYS -DUSE_TERMIO -DKLUDGELINEMODE -D_GNU_SOURCE
$(LDLIBS) += -ltermcap #mcli add

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS$(LDLIBS-$(@)))

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

$(OBJS): defs.h ext.h pathnames.h telnetd.h logwtmp.h logout.h

