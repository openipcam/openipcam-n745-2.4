.SUFFIXES : .x .c .o .s

BUILD_ROOT=/usr/local/arm_tools/
lib=${BUILD_ROOT}arm-elf/lib
lib1=${BUILD_ROOT}lib/gcc-lib/arm-elf/3.0/
include=${BUILD_ROOT}arm-elf/inc

CC=arm-elf-gcc -I${include} -I../../.. -I../../../include -I./lib -DNEW_KERNEL -DPLUGIN -O1 -Wl,-elf2flt
AS=arm-elf-as
LD=arm-elf-ld
AR=arm-elf-ar

WEC_LDFLAGS=-L./ -L${lib} -L${lib}/lib -L${lib}/libc  -L${lib1} 

OBJS =common.o  debug.o  discovery.o	if.o  plugin.o

LIBS=-lc -lgcc -lc

TARGET =libplugin.a

all: $(OBJS)
	$(AR) -rv $(TARGET) $(OBJS)
	cp $(TARGET) ../../

.o.c:
	$(CC) -c -o $*.o

.s.o:
	$(AS) -ahld -o $*.o $*.s > NULL
	del NULL

clean:
	rm -f *.a
	rm -f *.o 
	rm -f *.x 
	rm -f *.flat
