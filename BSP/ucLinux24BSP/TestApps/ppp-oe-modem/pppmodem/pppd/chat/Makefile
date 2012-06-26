
EXEC = chat
OBJS = chat.o

all: $(EXEC).x
	elf2flt -o $(EXEC) $(EXEC).x >tmp 2>&1

$(EXEC).x: $(OBJS)
	$(LD) -o $@ -T$(ROOTDIR)/user/elf2flt.ld $(OBJS) $(LDLIBS)
	rm -f $@
	$(LD) -o $@ $(LDFLAGS) $(OBJS) $(LDLIBS) -Map $(EXEC).map

.c.o:
	$(CC) $(CFLAGS) -c $*.c

.s.o:
	$(AS) -ahld -o $*.o $*.s > NULL
	del NULL

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

