#This bitch needs the following to build a v4 flat nommu exe file
#arm-elf-gcc  -O2 -fomit-frame-pointer   -Dlinux -D__linux__ -Dunix -D__uClinux__ -DEMBED -I/usr/local/arm_tools/arm-elf/inc -I/home/nuvoton/uClinux-dist/lib/include    -I/home/nuvoton/uClinux-dist -fno-builtin   -c -o gpio.o gpio.c
#arm-elf-gcc  -L/usr/local/arm_tools/arm-elf/lib -L.. -L/home/nuvoton/uClinux-dist/lib/lib -L/home/nuvoton/uClinux-dist/lib/libg -L/usr/local/arm_tools/lib/gcc-lib/arm-elf/3.0 -Wl,-elf2flt  -o gpio gpio.o -lc -lgcc -lc




EXEC = gpio
OBJS = gpio.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS$(LDLIBS-$(@)))

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o


