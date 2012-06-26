.SUFFIXES : .x .o .c .s

ROOT = /usr/local/arm_tools
LIB = $(ROOT)/lib/gcc-lib/arm-elf/3.0
LIB1 =$(ROOT)/arm-elf/lib
INC :=$(ROOT)/arm-elf/inc


CC=arm-elf-gcc -I$(INC) -Wl,-elf2flt

WEC_LDFLAGS=-L$(LIB) -L$(LIB1)

TARGET = audio_test

SRCS := audio_test.c

LIBS= -lc -lgcc -lc

all: 
	$(CC) $(WEC_LDFLAGS) $(SRCS) -o $(TARGET) $(LIBS)
	cp $(TARGET) ../../romdisk/bin
	genromfs -f romfs.img -d ../../romdisk/
	cp romfs.img ../../image
	
clean:
	rm -f *.o 
	rm -f *.x 
	rm -f *.flat
	rm -f *.map
	rm -f temp
	rm -f *.img
	rm -f $(TARGET)	
	rm -f *.gdb
