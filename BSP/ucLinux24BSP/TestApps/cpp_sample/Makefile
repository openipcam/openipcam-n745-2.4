.SUFFIXES : .x .o .cpp .s

BUILD_ROOT=/usr/local/arm_tools_3.3.4
lib1=${BUILD_ROOT}/lib/gcc-lib/arm-elf/3.3.4
lib=${BUILD_ROOT}/arm-elf/lib

include=${BUILD_ROOT}/arm-elf/include/

CC=${BUILD_ROOT}/bin/arm-elf-gcc -O1 -I$(include)  -D__uClinux__  -D__USE_BSD=1 -DRUN_ENVIRONMENT_LINUX -Wl,-elf2flt

AS=${BUILD_ROOT}/bin/arm-elf-as
LD=${BUILD_ROOT}/bin/arm-elf-ld

WEC_LDFLAGS=-L./ -L${lib} -L${lib1}

TARGET = hello
CPATH= ..
VPATH= .

APPOBJS:=$(patsubst %.cpp,%.o,$(wildcard ${VPATH}/*.cpp))
OBJS = ${APPOBJS}
APPSRCS:=$(wildcard ${VPATH}/*.cpp)

LIBS = -lc -lstdc++ -lgcc -lc -lstdc++ -lgcc -lstdc++ -lc -lgcc -lpthread -lc

all:
	$(CC) $(WEC_LDFLAGS) $(APPSRCS) -o $(TARGET) $(LIBS)
	cp $(TARGET) ../../romdisk/bin
	genromfs -f romfs.img -d ../../romdisk/
	cp romfs.img ../../image

clean:
	rm -f *.o
	rm -f *.x
	rm -f *.flat
	rm -f temp
	rm -f $(TARGET)



