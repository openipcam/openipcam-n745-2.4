.SUFFIXES : .x .o .c .s
BUILD_ROOT=/usr/local/arm_tools/
lib=${BUILD_ROOT}arm-elf/lib
lib1=${BUILD_ROOT}lib/gcc-lib/arm-elf/3.0/
include=${BUILD_ROOT}arm-elf/inc

CC=arm-elf-gcc -I${include} -I. -I./lib -I./include -Dlinux=1 -DHIGH_SPEED=1 -O2 -static  
AS=arm-elf-as
LD=arm-elf-ld
AR=arm-elf-ar

WEC_LDFLAGS=-L./ -L${lib} -L${lib}/lib -L${lib}/libc  -L${lib1}

OBJS = main.o mppp.o utils.o futils.o llist.o setget.o script.o

LIBS=-lc -lgcc -lc

TARGET =mppp


all: $(OBJS)
	$(CC) $(WEC_LDFLAGS) $(OBJS) -o $(TARGET) $(LIBS) -Wl,-elf2flt
	cp $(TARGET)  ../../../../romdisk/bin/
	genromfs   -f romfs.img  -d  ../../../../romdisk
	cp romfs.img  ../../../../image

.o.c:
	$(CC) -c -o $*.o
	
.s.o:
	$(AS) -ahld -o $*.o $*.s > NULL
	del NULL

	 
clean:
	rm -f *.o 
	rm -f *.x 
	rm -f temp
	rm -f *.map
	rm -f *.bak
	rm -f $(TARGET)
	rm -f *.img
	rm -f *.8	
	rm -f *.gdb	
