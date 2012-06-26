# $Id: Makefile,v 1.1.1.1 2006-07-11 09:33:47 andy Exp $
#
# Makefile for snefru wrapper
#
# Gene Kim
# Purdue University
# October 18, 1992
#

all:	snefru.o

.c.o:
	$(CC) $(CFLAGS) -c $<
	
clean:
	-rm -rf *.o .pure

snefru.o: ../../include/byteorder.h
