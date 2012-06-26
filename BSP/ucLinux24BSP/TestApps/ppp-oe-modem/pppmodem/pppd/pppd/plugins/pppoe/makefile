.SUFFIXES : .x .o .c .s
BUILD_ROOT=/usr/local/arm_tools/
#BUILD_ROOT=/home/zswan/arm_tools/
lib=${BUILD_ROOT}arm-elf/lib
lib1=${BUILD_ROOT}lib/gcc-lib/arm-elf/3.0/
include=${BUILD_ROOT}arm-elf/inc

CC=arm-elf-gcc -I${include}  -I./lib -I./include -Dlinux -D_linux_=1 -DPLUGIN -DPLUGIN_PPPOE  -DEMBED -O1 -c
AS=arm-elf-as
LD=arm-elf-ld
AR=arm-elf-ar

WEC_LDFLAGS=-L./ -L${lib} -L${lib}/lib -L${lib}/libc  -L${lib1} -T${lib}/lib/elf2flt.ld

OBJS =pppoehash.o pppoe_client.o pppoe_relay.o \
	  utils.o libpppoe.o pppoe.o pppoe_server.o

LIBS=-lc -lgcc -lc

all: libpppoe.a

libpppoe.a: $(OBJS)
	$(AR) -rv $@ $^
	cp libpppoe.a ../../
	
	
.c.o:
	$(CC) $*.c

.s.o:
	$(AS) -ahld -o $*.o $*.s > NULL
	del NULL
	 
clean:
	rm -f *.o 
	rm -f *.x 
	rm -f *.flat
	rm -f *.a

