# $Id: Makefile,v 1.1.1.1 2006-07-11 09:33:46 andy Exp $
#
# Makefile for crc32 wrapper
#
# Gene Kim
# Purdue University (now University of Arizona)
# July 10, 1994
#

all:	haval.o havalwrapper.o

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -f *.o .pure

haval.o: ../../include/byteorder.h
haval.o: haval.h
haval.o: havalapp.h
