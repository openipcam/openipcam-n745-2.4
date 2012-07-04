#/******************************************************************************
# *
# * Copyright (c) 2003 Windond Electronics Corp.
# * All rights reserved.
# *
# * Created by : PC31 WSChang
# ******************************************************************************/


CC=armcc
AR=armar
LD=armlink
FE=fromelf


INCL = ..\WBLv1_1\Inc\,..\WBLv1_1\Lib\

CCFLAGS =  -O2 -gtp -DUSE_SERIAL_PORT  -I$(INCL)
ARFLAGS = -c -r -create
LDFLAGS = -info sizes
FEFLAGS = -bin


OBJS_LI = uprintf_li.o serial_li.o
OBJS_BI = uprintf_bi.o serial_bi.o

all: uprintf_li.axf uprintf_bi.axf uprintf_li.a uprintf_bi.a
	copy *.a ..\WBLv1_1\Lib
	@if exist *.a echo [ Build completed ]

rebuild: clean all

clean:
	if exist *.o del *.o
	if exist *.a del *.a
	if exist *.axf del *.axf
	

uprintf_li.a: $(OBJS_LI)
	$(AR) $(ARFLAGS) uprintf_li.a $(OBJS_LI)

uprintf_bi.a: $(OBJS_BI)
	$(AR) $(ARFLAGS) uprintf_bi.a $(OBJS_BI)

uprintf_li.o: uprintf.c ..\WBLv1_1\Inc\serial.h
	$(CC) -c $(CCFLAGS) uprintf.c -o uprintf_li.o

uprintf_bi.o: uprintf.c ..\WBLv1_1\Inc\serial.h
	$(CC) -c $(CCFLAGS) -bi uprintf.c -o uprintf_bi.o

serial_li.o: serial.c ..\WBLv1_1\Inc\platform.h
	$(CC) -c $(CCFLAGS) serial.c -o serial_li.o

serial_bi.o: serial.c ..\WBLv1_1\Inc\platform.h
	$(CC) -c $(CCFLAGS) -bi serial.c -o serial_bi.o

uprintf_li.axf: ..\WBLv1_1\Lib\uprintf_li.a uprintf_test_li.o
	$(LD) ..\WBLv1_1\Lib\uprintf_li.a uprintf_test_li.o $(LDFLAGS) -o uprintf_li.axf
	$(FE) $(FEFLAGS) uprintf_li.axf -o uprintf_li.bin

uprintf_test_li.o: uprintf_test.c ..\WBLv1_1\Inc\platform.h ..\WBLv1_1\Inc\uprintf.h ..\WBLv1_1\Inc\serial.h
	$(CC) -c $(CCFLAGS) uprintf_test.c -o uprintf_test_li.o

uprintf_bi.axf: ..\WBLv1_1\Lib\uprintf_bi.a uprintf_test_bi.o
	$(LD) ..\WBLv1_1\Lib\uprintf_bi.a uprintf_test_bi.o $(LDFLAGS) -o uprintf_bi.axf
	$(FE) $(FEFLAGS) uprintf_bi.axf -o uprintf_bi.bin

uprintf_test_bi.o: uprintf_test.c ..\WBLv1_1\Inc\platform.h ..\WBLv1_1\Inc\uprintf.h ..\WBLv1_1\Inc\serial.h
	$(CC) -c $(CCFLAGS) -bi uprintf_test.c -o uprintf_test_bi.o

