# $Id: Makefile,v 1.1.1.1 2006-07-11 09:33:47 andy Exp $
#
# Makefile for MD2 wrapper
#
# Gene Kim
# Purdue University
# October 18, 1992
#

all:	md2wrapper.o md2.o

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -rf *.o .pure

md2.o: ../../include/byteorder.h
md2wrapper.o: ../../include/byteorder.h
