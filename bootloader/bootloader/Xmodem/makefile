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


INCL = ..\WBLv1_1\Inc\,..\WBLv1_1\Src\,..\WBLv1_1\Lib\

CCFLAGS = -O2 -gtp -I$(INCL)
ARFLAGS = -c -r -create
LDFLAGS = -entry Reset_Go -scatter ..\Embedded\scat_b.scf -nodebug 
FEFLAGS = -bin


all: Xmodem_bi.axf Xmodem_li.axf Xmodem_li.a Xmodem_bi.a
	copy *.a ..\WBLv1_1\Lib
	@if exist *.a echo [ Build completed ]

rebuild: clean all

clean:
	if exist *.o del *.o
	if exist *.a del *.a
	if exist *.axf del *.axf

Xmodem_li.a: Xmodem_li.o
	$(AR) $(ARFLAGS) Xmodem_li.a Xmodem_li.o


Xmodem_bi.a: Xmodem_bi.o
	$(AR) $(ARFLAGS) Xmodem_bi.a Xmodem_bi.o


Xmodem_li.axf: main_li.o xmodem_li.o ..\WBLv1_1\Lib\uprintf_li.a ..\WBLv1_1\Lib\flash_li.a ..\Embedded\Embedded_Data\little\Embedded.a
	$(LD) main_li.o xmodem_li.o ..\WBLv1_1\Lib\uprintf_li.a ..\WBLv1_1\Lib\flash_li.a ..\Embedded\Embedded_Data\little\Embedded.a $(LDFLAGS) -o Xmodem_li.axf
	$(FE) $(FEFLAGS) Xmodem_li.axf -o Xmodem_li.bin 


Xmodem_bi.axf: main_bi.o xmodem_bi.o ..\WBLv1_1\Lib\uprintf_bi.a ..\WBLv1_1\Lib\flash_bi.a ..\Embedded\Embedded_Data\big\Embedded.a
	$(LD) main_bi.o xmodem_bi.o ..\WBLv1_1\Lib\uprintf_bi.a ..\WBLv1_1\Lib\flash_bi.a ..\Embedded\Embedded_Data\big\Embedded.a $(LDFLAGS) -o Xmodem_bi.axf
	$(FE) $(FEFLAGS) Xmodem_bi.axf -o Xmodem_bi.bin 

main_li.o: main.c ..\WBLv1_1\Inc\platform.h ..\WBLv1_1\Inc\xmodem.h ..\WBLv1_1\Inc\flash.h ..\WBLv1_1\Inc\cdefs.h ..\WBLv1_1\Inc\serial.h ..\WBLv1_1\Inc\uprintf.h
	$(CC) -c $(CCFLAGS) main.c -o main_li.o

main_bi.o: main.c ..\WBLv1_1\Inc\platform.h ..\WBLv1_1\Inc\xmodem.h ..\WBLv1_1\Inc\flash.h ..\WBLv1_1\Inc\cdefs.h ..\WBLv1_1\Inc\serial.h ..\WBLv1_1\Inc\uprintf.h
	$(CC) -c $(CCFLAGS) -bi main.c -o main_bi.o

xmodem_li.o: xmodem.c ..\WBLv1_1\Inc\xmodem.h ..\WBLv1_1\Inc\uprintf.h
	$(CC) -c $(CCFLAGS) xmodem.c -o xmodem_li.o
	
xmodem_bi.o: xmodem.c ..\WBLv1_1\Inc\xmodem.h ..\WBLv1_1\Inc\uprintf.h
	$(CC) -c $(CCFLAGS) -bi xmodem.c -o xmodem_bi.o
	


