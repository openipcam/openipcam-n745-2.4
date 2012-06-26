.SUFFIXES : .x .c .o .s

BUILD_ROOT=/usr/local/arm_tools/
lib=${BUILD_ROOT}arm-elf/lib
lib1=${BUILD_ROOT}lib/gcc-lib/arm-elf/3.0/
include=${BUILD_ROOT}arm-elf/inc

CC=arm-elf-gcc -I${include}  -O1 -Wl,-elf2flt='-s 300000'

WEC_LDFLAGS=-L./ -L${lib} -L${lib}/lib -L${lib}/libc  -L${lib1} 

LIBS = -lc -lgcc -lpthread -lgcc -lc

TARGETS=ex1 ex2 ex3 ex4 ex5 ex6 ex7
all: $(TARGETS)
	flthdr -z ex1
	flthdr -z ex2
	flthdr -z ex3
	flthdr -z ex4
	flthdr -z ex5
	flthdr -z ex6
	flthdr -z ex7
	cp $(TARGETS) ../../romdisk/bin
	genromfs -f romfs.img -d ../../romdisk/
	cp romfs.img ../../image


ex1: ex1.o config.o
	$(CC) $(WEC_LDFLAGS) ex1.o config.o -o ex1 $(LIBS)


ex2: ex2.o config.o
	$(CC) $(WEC_LDFLAGS) ex2.o config.o -o ex2 $(LIBS)


ex3: ex3.o config.o
	$(CC) $(WEC_LDFLAGS) ex3.o config.o -o ex3 $(LIBS)


ex4: ex4.o config.o
	$(CC) $(WEC_LDFLAGS) ex4.o config.o -o ex4 $(LIBS)


ex5: ex5.o config.o
	$(CC) $(WEC_LDFLAGS) ex5.o config.o -o ex5 $(LIBS)


ex6: ex6.o config.o
	$(CC) $(WEC_LDFLAGS) ex6.o config.o -o ex6 $(LIBS)


ex7: ex7.o config.o
	$(CC) $(WEC_LDFLAGS) ex7.o config.o -o ex7 $(LIBS)


.c.o:
	$(CC) -c $<

clean:
	rm -f *.[oa] *~ core $(TARGETS) temp *.bak *.x *.gdb

