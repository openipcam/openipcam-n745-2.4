# $Id: Makefile,v 1.1.1.1 2006-07-11 09:33:46 andy Exp $
#
# Makefile for crc32 wrapper
#
# Gene Kim
# Purdue University
# October 18, 1992
#

all:	crc32.o

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -f *.o .pure

crc32.o: ../../include/byteorder.h
