.SUFFIXES : .x .o .c .s
#BUILD_ROOT=/usr/local/arm_tools/
BUILD_ROOT=/home/zswan/arm_tools/
lib=${BUILD_ROOT}arm-elf/lib
lib1=${BUILD_ROOT}lib/gcc-lib/arm-elf/3.0/
include=${BUILD_ROOT}arm-elf/inc

all:
	make -C plugins/pppoe
	make -f mfile

clean:
	make -C plugins/pppoe clean
	make -f mfile clean
