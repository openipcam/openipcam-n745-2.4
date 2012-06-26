.SUFFIXES : .x .c .o .s

BUILD_ROOT=/usr/local/arm_tools/
lib=${BUILD_ROOT}arm-elf/lib
lib1=${BUILD_ROOT}lib/gcc-lib/arm-elf/3.0/
include=${BUILD_ROOT}arm-elf/inc

CC=arm-elf-gcc -I../include -I${include} -I./lib -I./include -DPLUGIN -DNEW_KERNEL -DUSE_CRYPT -DCHAPMS=1 -DMPPE=1 -DHAVE_CRYPT_H=1 -O1 -Wl,-elf2flt
AS=arm-elf-as
LD=arm-elf-ld
AR=arm-elf-ar

WEC_LDFLAGS=-L./ -L${lib} -L${lib}/lib -L${lib}/libc  -L${lib1} 

OBJS =	main.o magic.o fsm.o lcp.o ipcp.o upap.o chap.o md5.o ccp.o ecp.o\
	ipxcp.o auth.o options.o sys-linux.o md4.o chap_ms.o inter.o libnat.o\
	demand.o utils.o tty.o eap.o pppcrypt.o sha1.o GetDefaultGateway.o

LIBS=-lplugin -lcrypt -lc -lgcc -lc

TARGET =pppd
	ROMDISK = ../../../../romdisk
	IMAGE = ../../../../image
all: $(OBJS)
	$(CC) $(WEC_LDFLAGS) $(OBJS) -o $(TARGET) $(LIBS)	
	cp $(TARGET) ../../../../romdisk/bin
	genromfs -f romfs.img -d $(ROMDISK)
	cp romfs.img $(IMAGE)

.o.c:
	$(CC) -c -o $*.o

.s.o:
	$(AS) -ahld -o $*.o $*.s > NULL
	del NULL

	 
clean:
#	make -C plugins/rp-pppoe clean
	rm -f *.o 
	rm -f *.gdb
	rm -f *.x 
	rm -f *.flat
	rm -f $(TARGET)
	rm -f *.img
	rm -f *.map
	rm -f temp
	rm -f *.a
