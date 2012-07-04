## set ARM folders (assumes we use the arm-elf-gcc 3.0 defaults)
ROOT=/usr/local/bin/arm_tools
LIB1=$ROOT/arm-elf/lib
LIB2=$ROOT/lib/gcc-lib/arm-elf/3.0
INC=$ROOT/arm-elf/inc

CC="arm-elf-gcc -mcpu=arm7tdmi "
CFLAGS="-Wall -I$INC"
LDLIBS="-lc -lgcc -lcrypt -L$LIB1 -L$LIB2"
UCFLAGS="-Wl,-elf2flt "

export ROOT LIB1 LIB2 INC CC CFLAGS LDLIBS UCFLAGS DEVICE
make
