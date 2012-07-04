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


INCL = W90N740\Src\,..\WBLv1_1\Inc\,..\WBLv1_1\Src\,..\WBLv1_1\Lib\

CCFLAGS = -O2 -gtp -I$(INCL)
ARFLAGS = -c -r -create
LDFLAGS = -entry Reset_Go -scatter ..\Embedded\scat_b.scf -nodebug 
FEFLAGS = -bin


all: TFTPserv_li.axf TFTPserv_bi.axf TFTPserv_li.a TFTPserv_bi.a
	copy *.a ..\WBLv1_1\Lib
	@if exist *.a echo [ Build completed ]

rebuild: clean all

clean:
	if exist *.o del *.o
	if exist *.a del *.a
	if exist *.axf del *.axf

TFTPserv_li.a: netbuf_li.o dhcp_li.o PacketDriver_li.o TftpServer_li.o NetKernel_li.o
	$(AR) $(ARFLAGS) TFTPserv_li.a netbuf_li.o dhcp_li.o PacketDriver_li.o TftpServer_li.o NetKernel_li.o

TFTPserv_bi.a: netbuf_bi.o dhcp_bi.o PacketDriver_bi.o TftpServer_bi.o NetKernel_bi.o
	$(AR) $(ARFLAGS) TFTPserv_bi.a netbuf_bi.o dhcp_bi.o PacketDriver_bi.o TftpServer_bi.o NetKernel_bi.o

TFTPserv_li.axf: ..\WBLv1_1\Lib\TFTPserv_li.a ..\WBLv1_1\Lib\uprintf_li.a tftp_test_li.o ..\WBLv1_1\Lib\flash_li.a ..\Embedded\Embedded_Data\little\Embedded.a
	$(LD) ..\WBLv1_1\Lib\TFTPserv_li.a ..\WBLv1_1\Lib\uprintf_li.a tftp_test_li.o ..\WBLv1_1\Lib\flash_li.a ..\Embedded\Embedded_Data\little\Embedded.a $(LDFLAGS) -o TFTPserv_li.axf
	$(FE) $(FEFLAGS) TFTPserv_li.axf -o TFTPserv_li.bin 

TFTPserv_bi.axf: ..\WBLv1_1\Lib\TFTPserv_bi.a ..\WBLv1_1\Lib\uprintf_bi.a tftp_test_bi.o ..\WBLv1_1\Lib\flash_bi.a ..\Embedded\Embedded_Data\big\Embedded.a
	$(LD) ..\WBLv1_1\Lib\TFTPserv_bi.a ..\WBLv1_1\Lib\uprintf_bi.a tftp_test_bi.o ..\WBLv1_1\Lib\flash_bi.a ..\Embedded\Embedded_Data\big\Embedded.a $(LDFLAGS) -o TFTPserv_bi.axf
	$(FE) $(FEFLAGS) TFTPserv_bi.axf -o TFTPserv_bi.bin 

netbuf_li.o: W90N740\Src\netbuf.c W90N740\Src\W90N740.h W90N740\Src\netbuf.h
	$(CC) -c $(CCFLAGS) W90N740\Src\netbuf.c -o netbuf_li.o

dhcp_li.o: W90N740\Src\dhcp.c W90N740\Src\netbuf.h W90N740\Src\net.h W90N740\Src\dhcp.h
	$(CC) -c $(CCFLAGS) W90N740\Src\dhcp.c -o dhcp_li.o

PacketDriver_li.o: W90N740\Src\PacketDriver.c W90N740\Src\netbuf.h W90N740\Src\w90n740.h W90N740\Src\net.h
	$(CC) -c $(CCFLAGS) W90N740\Src\PacketDriver.c -o PacketDriver_li.o

TftpServer_li.o: W90N740\Src\TftpServer.c W90N740\Src\net.h W90N740\Src\netbuf.h
	$(CC) -c $(CCFLAGS) W90N740\Src\TftpServer.c -o TftpServer_li.o

NetKernel_li.o: W90N740\Src\NetKernel.c W90N740\Src\W90N740.h W90N740\Src\netbuf.h W90N740\Src\net.h
	$(CC) -c $(CCFLAGS) W90N740\Src\NetKernel.c -o NetKernel_li.o

netbuf_bi.o: W90N740\Src\netbuf.c W90N740\Src\W90N740.h W90N740\Src\netbuf.h
	$(CC) -c $(CCFLAGS) -bi W90N740\Src\netbuf.c -o netbuf_bi.o

dhcp_bi.o: W90N740\Src\dhcp.c W90N740\Src\netbuf.h W90N740\Src\net.h W90N740\Src\dhcp.h
	$(CC) -c $(CCFLAGS) -bi W90N740\Src\dhcp.c -o dhcp_bi.o

PacketDriver_bi.o: W90N740\Src\PacketDriver.c W90N740\Src\netbuf.h W90N740\Src\w90n740.h W90N740\Src\net.h
	$(CC) -c $(CCFLAGS) -bi W90N740\Src\PacketDriver.c -o PacketDriver_bi.o

TftpServer_bi.o: W90N740\Src\TftpServer.c W90N740\Src\net.h W90N740\Src\netbuf.h
	$(CC) -c $(CCFLAGS) -bi W90N740\Src\TftpServer.c -o TftpServer_bi.o

NetKernel_bi.o: W90N740\Src\NetKernel.c W90N740\Src\W90N740.h W90N740\Src\netbuf.h W90N740\Src\net.h
	$(CC) -c $(CCFLAGS) -bi W90N740\Src\NetKernel.c -o NetKernel_bi.o

tftp_test_li.o: W90N740\Src\tftp_test.c W90N740\Src\tftp.h
	$(CC) -c $(CCFLAGS) W90N740\Src\tftp_test.c -o tftp_test_li.o

tftp_test_bi.o: W90N740\Src\tftp_test.c W90N740\Src\tftp.h
	$(CC) -c $(CCFLAGS) -bi W90N740\Src\tftp_test.c -o tftp_test_bi.o

